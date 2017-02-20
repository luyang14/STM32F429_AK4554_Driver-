//--------------------------------------------------------------
// File     : stm32f4_led.c
// Datum    : 24.10.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F429
// IDE      : CooCox CoIDE 1.7.4
// GCC      : 4.7 2012q4
// Module   : GPIO
// Funktion : LED Funktionen
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4_led.h"


//--------------------------------------------------------------
// Definition aller LEDs
// Reihenfolge wie bei LED_NAME_t
//
// Init : [LED_OFF,LED_ON]
//--------------------------------------------------------------
LED_t LED[] = {
  // Name    ,PORT , PIN       , CLOCK              , Init
  {LED_GREEN ,GPIOB,GPIO_Pin_10,RCC_AHB1Periph_GPIOB,LED_OFF},   // PB10=Gruene LED auf dem Discovery-Board
  {LED_RED   ,GPIOE,GPIO_Pin_15,RCC_AHB1Periph_GPIOE,LED_OFF},   // PE15=Rote LED auf dem Discovery-Board
};



//--------------------------------------------------------------
// Init aller LEDs
//--------------------------------------------------------------
void UB_Led_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  LED_NAME_t led_name;
	for(led_name=0;led_name<LED_ANZ;led_name++)
		{
			// Clock Enable
			RCC_AHB1PeriphClockCmd(LED[led_name].LED_CLK, ENABLE);

			// Config als Digital-Ausgang
			GPIO_InitStructure.GPIO_Pin = LED[led_name].LED_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(LED[led_name].LED_PORT, &GPIO_InitStructure);

			// Default Wert einstellen
			if(LED[led_name].LED_INIT==LED_OFF) 
				{
					UB_Led_Off(led_name);
				}
			else 
				{
					UB_Led_On(led_name);
				}
		}
}


//--------------------------------------------------------------
// LED ausschalten
//--------------------------------------------------------------
void UB_Led_Off(LED_NAME_t led_name)
{
  LED[led_name].LED_PORT->BSRRH = LED[led_name].LED_PIN;
}

//--------------------------------------------------------------
// LED einschalten
//--------------------------------------------------------------
void UB_Led_On(LED_NAME_t led_name)
{
  LED[led_name].LED_PORT->BSRRL = LED[led_name].LED_PIN;
} 

//--------------------------------------------------------------
// LED toggeln
//--------------------------------------------------------------
void UB_Led_Toggle(LED_NAME_t led_name)
{
  LED[led_name].LED_PORT->ODR ^= LED[led_name].LED_PIN;
}

//--------------------------------------------------------------
// LED ein- oder ausschalten
//--------------------------------------------------------------
void UB_Led_Switch(LED_NAME_t led_name, LED_STATUS_t wert)
{
  if(wert==LED_OFF) {
    UB_Led_Off(led_name);
  }
  else {
    UB_Led_On(led_name);
  }
}
