#ifndef __I2S_H
#define __I2S_H
   									
#include "stm32f4xx.h"

#define AUDIO_OUT_PACKET                              (uint32_t)(((ma_AudioFreq * 2 * 2) /1000))

extern void (*i2s_tx_callback)(void);		//IIS TX回调函数指针  
extern __IO uint8_t I2S_Rx_Buffer0[];
extern __IO uint8_t I2S_Tx_Buffer0[];
extern u16 sample_mic;
extern u16 sample_audio;

void I2S3_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat,u32 _AudioFreq); 
void I2S3_DMA_Init(u16 num);
u8 I2S3_SampleRate_Set(u32 samplerate);
void Exchange(u8 *usartbuf ,u16 *i2sbuf,u16 usartbufnum,u8 master_or_slave);
void I2S_Play_Start(void); 
void I2S_Play_Stop(void);  
#endif





















