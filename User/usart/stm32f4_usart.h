#ifndef STM32F4_USART_H
#define STM32F4_USART_H

#include "stdio.h"	
#include "stm32f4xx.h"
 
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			  1		  //ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		    //����״̬���	
extern uint8_t USART1_Tx_DMA_Buffer[];
extern uint8_t USART1_Rx_DMA_Buffer[];
extern uint8_t Flag_USART1_RX_Finish;
extern uint8_t Flag_USART1_TX_Finish;

void uart_init(u32 bound);
void NVIC_Configuration(void);
void USART1_DMA_Config(void);

#endif
