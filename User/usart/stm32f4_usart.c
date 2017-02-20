/********************     (C) COPYRIGHT 2017     **************************
 * �ļ���  ��stm32f4_usart.c
 * ����    ��usart1��������       
 * ʵ��ƽ̨��STM32F429ZGT6
 * ��汾  ��V1.6.1
 *
 * ��д���ڣ�2017-02-07
 * �޸����ڣ�
 * ����    :
 ***************************************************************************/
#include "stm32f4_usart.h"	
#include "stm32f4_i2s.h"

#if EN_USART1_RX   //���ʹ���˽���

#define USART1_RX_DMA_BUF_SIZE 6
#define USART1_TX_DMA_BUF_SIZE 6
uint8_t USART1_Tx_DMA_Buffer[USART1_TX_DMA_BUF_SIZE];
uint8_t USART1_Rx_DMA_Buffer[USART1_RX_DMA_BUF_SIZE];
uint8_t Flag_USART1_TX_Finish=0;
uint8_t Flag_USART1_RX_Finish=0;

/***************************************************************
 *���� ��uart_init
 *���� ����ʼ��IO ����1 
 *���� ��bound:������
 *��� ����
 ***************************************************************/
void uart_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��

	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1

	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10
	
	/*USART1 ��ʼ������*/
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1

	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 

//	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//��������ж�
	
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
 *���� ��USART1_IRQHandler
 *���� ������1�жϷ�����
 *���� ����
 *��� ����
 ***************************************************************/

void USART1_IRQHandler(void) 
{   
	static u8 Data;
	static u8 LastData;
	static u8 i=0;
	static u8 Save_Start=0;
//  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)  //�����ж�
//	{
//		for(i=0;i<2;i++)
//		{
//		  USART_SendData(USART1,buffer[i] );
//		  //while (!(USART1->SR & USART_FLAG_TXE));	
//		}	
//  } 	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Data =USART1->DR;	//��ȡ���յ�������  
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

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

