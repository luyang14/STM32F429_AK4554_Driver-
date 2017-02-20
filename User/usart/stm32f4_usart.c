/********************     (C) COPYRIGHT 2017     **************************
 * 文件名  ：stm32f4_usart.c
 * 描述    ：usart1测试例程       
 * 实验平台：STM32F429ZGT6
 * 库版本  ：V1.6.1
 *
 * 编写日期：2017-02-07
 * 修改日期：
 * 作者    :
 ***************************************************************************/
#include "stm32f4_usart.h"	
#include "stm32f4_i2s.h"

#if EN_USART1_RX   //如果使能了接收

#define USART1_RX_DMA_BUF_SIZE 6
#define USART1_TX_DMA_BUF_SIZE 6
uint8_t USART1_Tx_DMA_Buffer[USART1_TX_DMA_BUF_SIZE];
uint8_t USART1_Rx_DMA_Buffer[USART1_RX_DMA_BUF_SIZE];
uint8_t Flag_USART1_TX_Finish=0;
uint8_t Flag_USART1_RX_Finish=0;

/***************************************************************
 *函数 ：uart_init
 *功能 ：初始化IO 串口1 
 *输入 ：bound:波特率
 *输出 ：无
 ***************************************************************/
void uart_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟

	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1

	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
	
	/*USART1 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1

	USART_Cmd(USART1, ENABLE);  //使能串口1 

//	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//开启相关中断
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	NVIC_Configuration();
}
/******************************************************************************* 
* Function Name  : NVIC_Configuration 
* Description    : None
* Input          : None 
* Output         : None 
* Return         : None 
*******************************************************************************/  

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
//    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);   
#endif  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
}

/***************************************************************
 *函数 ：USART1_IRQHandler
 *功能 ：串口1中断服务函数
 *输入 ：无
 *输出 ：无
 ***************************************************************/

void USART1_IRQHandler(void) 
{   
	static u8 Data;
	static u8 LastData;
	static u8 i=0;
	static u8 Save_Start=0;
//  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)  //发送中断
//	{
//		for(i=0;i<2;i++)
//		{
//		  USART_SendData(USART1,buffer[i] );
//		  //while (!(USART1->SR & USART_FLAG_TXE));	
//		}	
//  } 	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		Data =USART1->DR;	//读取接收到的数据  
    if((LastData==0xEF)&&(Data==0x10))
		{
			i=0;
			Save_Start=1;
		}
		if(Save_Start==1)
		{
			USART1_Rx_DMA_Buffer[i]=0;
      USART1_Rx_DMA_Buffer[i++]=Data;
      if(i==5)
				{
					Flag_USART1_RX_Finish=1;
					Save_Start=0;
					USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
				}					
		}			
		LastData=Data;
	}
	//USART_ClearFlag(USART1, USART_FLAG_TC);
} 

#endif	

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

