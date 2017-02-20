/**
  ******************************************************************************
  * @file    main.c
  * @author  
  * @version V1.0.0
  * @date    7-2-2017
  * @brief            
  * @verbatim
  * @endverbatim
  ******************************************************************************
  * @attention
	*
	*
  ******************************************************************************  
	*/ 

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "stm32f4_ak4554.h"
#include "stm32f4_usart.h"
#include "stm32f4_led.h"
#include "stm32f4_Tim2.h"
#include "stm32f4_i2s.h"

void Delay(__IO uint32_t nTime)
{ 
 
  while(--nTime != 0);
}

int main()
{ 
	u8 i=0;

  UB_Led_Init();
	uart_init(4000000);
	AK4554_init();
	TIM2_Config();
	EVAL_AUDIO_Init(ma_AudioFreq);

	while(1)
	{
		if(Flag_USART1_RX_Finish==1)
		{
      sample_audio=0;
			sample_audio=USART1_Rx_DMA_Buffer[2];
			sample_audio<<=8;
			sample_audio|=USART1_Rx_DMA_Buffer[1];
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
			SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);
			Flag_USART1_RX_Finish=0;
		}
		
		if(Flag_USART1_TX_Finish==1)
		{
			USART1_Tx_DMA_Buffer[0]=0xEF;
		  USART1_Tx_DMA_Buffer[1]=0x10;
			USART1_Tx_DMA_Buffer[4]=0xAA;
		  USART1_Tx_DMA_Buffer[5]=0xBB;
			for(i=0;i<6;)
			{
				while((USART1->SR & 0x0080) == (uint16_t)RESET)
					;
				USART_SendData(USART1, USART1_Tx_DMA_Buffer[i]);
				i++;
			}
			Flag_USART1_TX_Finish=0;
	  }
//GPIO_SetBits(GPIOE,GPIO_Pin_15);
//Delay(0x2FFFFF);
//GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	}
}
