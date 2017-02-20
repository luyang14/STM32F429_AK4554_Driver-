#ifndef STM32F4_USART_H
#define STM32F4_USART_H

#include "stdio.h"	
#include "stm32f4xx.h"
 
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			  1		  //使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		    //接收状态标记	
extern uint8_t USART1_Tx_DMA_Buffer[];
extern uint8_t USART1_Rx_DMA_Buffer[];
extern uint8_t Flag_USART1_RX_Finish;
extern uint8_t Flag_USART1_TX_Finish;

void uart_init(u32 bound);
void NVIC_Configuration(void);
void USART1_DMA_Config(void);

#endif
