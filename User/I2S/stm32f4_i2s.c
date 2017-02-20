#include "stm32f4_i2s.h"  
#include "stm32f4_usart.h"
#include "stm32f4_ak4554.h"

//�����ʼ��㹫ʽ:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//һ��HSE=8Mhz 
//pllm:��Sys_Clock_Set���õ�ʱ��ȷ����һ����8
//PLLI2SN:һ����192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S��Ƶϵ����@pllm=8,HSE=8Mhz,��vco����Ƶ��Ϊ1Mhz
//���ʽ:������/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz������
	{1102,429,4,19,0},		//11.025Khz������ 
	{1600,213,2,13,0},		//16Khz������
	{2205,429,4, 9,1},		//22.05Khz������
	{3200,213,2, 6,1},		//32Khz������
	{4410,271,2, 6,0},		//44.1Khz������
	{4800,258,3, 3,1},		//48Khz������
	{8820,316,2, 3,1},		//88.2Khz������
	{9600,344,2, 3,1},  	//96Khz������
	{17640,361,2,2,0},  	//176.4Khz������ 
	{19200,393,2,2,0},  	//192Khz������
}; 

/*
*********************************************************************************************************
*	�� �� ��: I2S_NVIC_Config
*	����˵��: ����I2S NVICͨ��(�ж�ģʽ)���жϷ�����void SPI3_IRQHandler(void) ��stm32f10x_it.c
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void I2S_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* SPI3 IRQ ͨ������ */
	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/********************************************************************************************************
 * ��������I2S3_Init
 * ����  ��I2S3��ʼ��
 * ����  : I2S_Standard: @SPI_I2S_Standard  I2S��׼,
 *                            I2S_Standard_Phillips,�����ֱ�׼;
 *                            I2S_Standard_MSB,MSB�����׼(�Ҷ���);
 *                            I2S_Standard_LSB,LSB�����׼(�����);
 *                            I2S_Standard_PCMShort,
 *														 I2S_Standard_PCMLong:PCM��׼
 *         I2S_Mode: @SPI_I2S_Mode  I2S_Mode_SlaveTx:�ӻ�����;
 *                                  I2S_Mode_SlaveRx:�ӻ�����;
 *																	 I2S_Mode_MasterTx:��������;
 *																	 I2S_Mode_MasterRx:��������;
 *         I2S_Clock_Polarity  &SPI_I2S_Clock_Polarity:  I2S_CPOL_Low,ʱ�ӵ͵�ƽ��Ч;
 *                                                       I2S_CPOL_High,ʱ�Ӹߵ�ƽ��Ч
 *         I2S_DataFormat��@SPI_I2S_Data_Format :���ݳ���,I2S_DataFormat_16b,16λ��׼;
 *                                                        I2S_DataFormat_16bextended,16λ��չ(frame=32bit);
 *																												I2S_DataFormat_24b,24λ;
 *																												I2S_DataFormat_32b,32λ.
 * ��� ����
 *********************************************************************************************************/
void I2S3_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat,u32 _AudioFreq)
{ 
  I2S_InitTypeDef I2S_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//ʹ��SPI3ʱ��
  
	SPI_I2S_DeInit(SPI3);
	
  I2S_InitStructure.I2S_Mode=I2S_Mode;                   //IISģʽ
  I2S_InitStructure.I2S_Standard=I2S_Standard;           //IIS��׼
  I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;       //IIS���ݳ���
  I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Enable;//��ʱ�����ʹ��I2S_MCLKOutput_Disable
  I2S_InitStructure.I2S_AudioFreq=_AudioFreq;            //IISƵ������
  I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;         //����״̬ʱ�ӵ�ƽ
  I2S_Init(SPI3,&I2S_InitStructure);                     //��ʼ��IIS
  
	/* Configure the I2Sx_ext (the second instance) in Slave Receiver Mode */
	I2S_FullDuplexConfig(I2S3ext, &I2S_InitStructure);

  I2S_Cmd(SPI3,ENABLE);                                  //SPI3 I2S ENʹ��.	
	
  /* Enable the I2Sx_ext peripheral for Full Duplex mode */
	I2S_Cmd(I2S3ext, ENABLE);
	
	I2S_NVIC_Config();
	
	/* ��ֹI2S3 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
	//SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, DISABLE);
  SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);
	/* ��ֹI2S3 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
	SPI_I2S_ITConfig(I2S3ext, SPI_I2S_IT_RXNE, ENABLE);
} 

/**********************************
 * ������ : I2S2_SampleRate_Set
 * ����   ������IIS�Ĳ�����(@MCKEN)
 * ����   ��samplerate:������,��λ:Hz
 * ���   : 0,���óɹ�;1,�޷�����.
 **********************************/
u8 I2S3_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	
	samplerate/=10;//��С10��   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC_PLLI2SCmd(DISABLE);//�ȹر�PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
	RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//����I2SxCLK��Ƶ��(x=2)  ����PLLI2SN PLLI2SR
	RCC->CR|=1<<26;					      //����I2Sʱ��
	while((RCC->CR&1<<27)==0);		//�ȴ�I2Sʱ�ӿ����ɹ�. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//����I2SDIV
	tempreg|=I2S_PSC_TBL[i][3]<<8;//����ODDλ
	tempreg|=1<<9;					      //ʹ��MCKOEλ,���MCK
	SPI2->I2SPR=tempreg;			    //����I2SPR�Ĵ��� 
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
	if (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == SET)//���ͻ�����Ϊ��
	{
		SPI_I2S_SendData(SPI3,sample_audio);
		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, DISABLE);
	}
	if (SPI_I2S_GetFlagStatus(I2S3ext, SPI_I2S_FLAG_RXNE) != RESET)//���ջ������ǿ�
	{ 
		sample_mic=0;
		sample_mic = SPI_I2S_ReceiveData(I2S3ext);
		USART1_Tx_DMA_Buffer[3]=sample_mic>>8;
		USART1_Tx_DMA_Buffer[2]=sample_mic;
		Flag_USART1_TX_Finish=1;
	}
}

