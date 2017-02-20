#include "stm32f4_ak4554.h"	
#include "stm32f4_i2s.h" 

/**********************************
 * 函数名 : AK4554_init
 * 功能   ：音频接口初始化
 * 输入   ：无
 * 输出   : 无
 **********************************/
void AK4554_init(void)
	{
		//GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;
			
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
	
		//端口配置MCLK:PC7 SCLK:PC10 SDTO:PC11 SDTI:PC12
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC7,PC10,PC12,PC11
		//端口配置LRCK:PA15	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA15
		
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_SPI3);  //PA15,AF6  I2S_LRCK
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3);	//PC10,AF6  I2S_SCLK 
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);	//PC12,AF6  I2S_DACDATA 
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_SPI3);	  //PC7 ,AF6  I2S_MCK
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3);	//PC11,AF6  I2S_ADCDATA 
	}

/**********************************
 * 函数名 : EVAL_AUDIO_Init
 * 功能   ：配置音频接口
 * 输入   ：AudioFreq:音频采样率,单位:Hz
 * 输出   : 0,设置成功;1,无法设置.
 **********************************/	
uint32_t EVAL_AUDIO_Init(uint32_t AudioFreq)
{   
	I2S3_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_High,I2S_DataFormat_16b,ma_AudioFreq);//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度
	I2S3_SampleRate_Set(AudioFreq);		//设置采样率
	return 0; 
}
