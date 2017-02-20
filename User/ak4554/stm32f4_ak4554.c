#include "stm32f4_ak4554.h"	
#include "stm32f4_i2s.h" 

/**********************************
 * ������ : AK4554_init
 * ����   ����Ƶ�ӿڳ�ʼ��
 * ����   ����
 * ���   : ��
 **********************************/
void AK4554_init(void)
	{
		//GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;
			
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOAʱ��
	
		//�˿�����MCLK:PC7 SCLK:PC10 SDTO:PC11 SDTI:PC12
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC7,PC10,PC12,PC11
		//�˿�����LRCK:PA15	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA15
		
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_SPI3);  //PA15,AF6  I2S_LRCK
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3);	//PC10,AF6  I2S_SCLK 
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);	//PC12,AF6  I2S_DACDATA 
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_SPI3);	  //PC7 ,AF6  I2S_MCK
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3);	//PC11,AF6  I2S_ADCDATA 
	}

/**********************************
 * ������ : EVAL_AUDIO_Init
 * ����   ��������Ƶ�ӿ�
 * ����   ��AudioFreq:��Ƶ������,��λ:Hz
 * ���   : 0,���óɹ�;1,�޷�����.
 **********************************/	
uint32_t EVAL_AUDIO_Init(uint32_t AudioFreq)
{   
	I2S3_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_High,I2S_DataFormat_16b,ma_AudioFreq);//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����
	I2S3_SampleRate_Set(AudioFreq);		//���ò�����
	return 0; 
}
