#include "stm32f4_i2s.h"  
#include "stm32f4_usart.h"
#include "stm32f4_ak4554.h"

//采样率计算公式:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//一般HSE=8Mhz 
//pllm:在Sys_Clock_Set设置的时候确定，一般是8
//PLLI2SN:一般是192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S分频系数表@pllm=8,HSE=8Mhz,即vco输入频率为1Mhz
//表格式:采样率/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz采样率
	{1102,429,4,19,0},		//11.025Khz采样率 
	{1600,213,2,13,0},		//16Khz采样率
	{2205,429,4, 9,1},		//22.05Khz采样率
	{3200,213,2, 6,1},		//32Khz采样率
	{4410,271,2, 6,0},		//44.1Khz采样率
	{4800,258,3, 3,1},		//48Khz采样率
	{8820,316,2, 3,1},		//88.2Khz采样率
	{9600,344,2, 3,1},  	//96Khz采样率
	{17640,361,2,2,0},  	//176.4Khz采样率 
	{19200,393,2,2,0},  	//192Khz采样率
}; 

/*
*********************************************************************************************************
*	函 数 名: I2S_NVIC_Config
*	功能说明: 配置I2S NVIC通道(中断模式)。中断服务函数void SPI3_IRQHandler(void) 在stm32f10x_it.c
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void I2S_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* SPI3 IRQ 通道配置 */
	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/********************************************************************************************************
 * 函数名：I2S3_Init
 * 描述  ：I2S3初始化
 * 输入  : I2S_Standard: @SPI_I2S_Standard  I2S标准,
 *                            I2S_Standard_Phillips,飞利浦标准;
 *                            I2S_Standard_MSB,MSB对齐标准(右对齐);
 *                            I2S_Standard_LSB,LSB对齐标准(左对齐);
 *                            I2S_Standard_PCMShort,
 *														 I2S_Standard_PCMLong:PCM标准
 *         I2S_Mode: @SPI_I2S_Mode  I2S_Mode_SlaveTx:从机发送;
 *                                  I2S_Mode_SlaveRx:从机接收;
 *																	 I2S_Mode_MasterTx:主机发送;
 *																	 I2S_Mode_MasterRx:主机接收;
 *         I2S_Clock_Polarity  &SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,时钟低电平有效;
 *                                                       I2S_CPOL_High,时钟高电平有效
 *         I2S_DataFormat：@SPI_I2S_Data_Format :数据长度,I2S_DataFormat_16b,16位标准;
 *                                                        I2S_DataFormat_16bextended,16位扩展(frame=32bit);
 *																												I2S_DataFormat_24b,24位;
 *																												I2S_DataFormat_32b,32位.
 * 输出 ：无
 *********************************************************************************************************/
void I2S3_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat,u32 _AudioFreq)
{ 
  I2S_InitTypeDef I2S_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPI3时钟
  
	SPI_I2S_DeInit(SPI3);
	
  I2S_InitStructure.I2S_Mode=I2S_Mode;                   //IIS模式
  I2S_InitStructure.I2S_Standard=I2S_Standard;           //IIS标准
  I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;       //IIS数据长度
  I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Enable;//主时钟输出使能I2S_MCLKOutput_Disable
  I2S_InitStructure.I2S_AudioFreq=_AudioFreq;            //IIS频率设置
  I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;         //空闲状态时钟电平
  I2S_Init(SPI3,&I2S_InitStructure);                     //初始化IIS
  
	/* Configure the I2Sx_ext (the second instance) in Slave Receiver Mode */
	I2S_FullDuplexConfig(I2S3ext, &I2S_InitStructure);

  I2S_Cmd(SPI3,ENABLE);                                  //SPI3 I2S EN使能.	
	
  /* Enable the I2Sx_ext peripheral for Full Duplex mode */
	I2S_Cmd(I2S3ext, ENABLE);
	
	I2S_NVIC_Config();
	
	/* 禁止I2S3 TXE中断(发送缓冲区空)，需要时再打开 */
	//SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, DISABLE);
  SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);
	/* 禁止I2S3 RXNE中断(接收不空)，需要时再打开 */
	SPI_I2S_ITConfig(I2S3ext, SPI_I2S_IT_RXNE, ENABLE);
} 

/**********************************
 * 函数名 : I2S2_SampleRate_Set
 * 功能   ：设置IIS的采样率(@MCKEN)
 * 输入   ：samplerate:采样率,单位:Hz
 * 输出   : 0,设置成功;1,无法设置.
 **********************************/
u8 I2S3_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	
	samplerate/=10;//缩小10倍   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC_PLLI2SCmd(DISABLE);//先关闭PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
	RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//设置I2SxCLK的频率(x=2)  设置PLLI2SN PLLI2SR
	RCC->CR|=1<<26;					      //开启I2S时钟
	while((RCC->CR&1<<27)==0);		//等待I2S时钟开启成功. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//设置I2SDIV
	tempreg|=I2S_PSC_TBL[i][3]<<8;//设置ODD位
	tempreg|=1<<9;					      //使能MCKOE位,输出MCK
	SPI2->I2SPR=tempreg;			    //设置I2SPR寄存器 
	return 0;
} 

/*************************************************
 * @brief  This function handles SPI3 Handler.
 * @param  None
 * @retval None
 *************************************************/
u16 sample_audio = 0;
u16 sample_mic = 0;
void SPI3_IRQHandler(void)
{	 
	if (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == SET)//发送缓冲区为空
	{
		SPI_I2S_SendData(SPI3,sample_audio);
		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, DISABLE);
	}
	if (SPI_I2S_GetFlagStatus(I2S3ext, SPI_I2S_FLAG_RXNE) != RESET)//接收缓冲区非空
	{ 
		sample_mic=0;
		sample_mic = SPI_I2S_ReceiveData(I2S3ext);
		USART1_Tx_DMA_Buffer[3]=sample_mic>>8;
		USART1_Tx_DMA_Buffer[2]=sample_mic;
		Flag_USART1_TX_Finish=1;
	}
}

