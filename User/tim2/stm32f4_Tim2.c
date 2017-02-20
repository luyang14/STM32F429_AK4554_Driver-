/********************     (C) COPYRIGHT 2017     **************************
 * �ļ���  ��Tim2.c
 * ����    ����ʱ��timer2 ��������       
 * ʵ��ƽ̨��STM32F429ZGT6
 * ��汾  ��V1.6.1
 *
 * ��д���ڣ�2017-02-07
 * �޸����ڣ�
 * ����    :
****************************************************************************/
#include "stm32f4_Tim2.h"

/**********************************
 * ��������TIM2_NVIC_Configuration
 * ����  ��TIM2�ж����ȼ�����
 * ����  ����
 * ���  ����	
 **********************************/
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/********************************************************************
 * ��������TIM2_Configuration
 * ����  ��TIM_Period--1000   TIM_Prescaler--(22500-1) -->�ж�����Ϊ
 * ((1/180000000)*8)*22500*1000=1S    1�붨ʱ��
 * ����  ����
 * ���  ����	
 ********************************************************************/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=1000;		 // �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= (22500 - 1);	// ʱ��Ԥ��Ƶ�� (180M/8)/22500 
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		// ������Ƶ 
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // ���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							// �������жϱ�־ 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);														// ����ʱ�� 
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);		// �ȹرյȴ�ʹ��
}
/**************************
 * ��������TIM2_Config
 * ����  ��TIM2����
 * ����  ����
 * ���  ����	
 **************************/
void TIM2_Config(void)
{
  TIM2_Configuration();
	TIM2_NVIC_Configuration();
}
/**********************************************
 * @brief  This function handles TIM2 Handler.
 * @param  None
 * @retval None
 **********************************************/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{   
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
		GPIO_WriteBit(GPIOE,GPIO_Pin_15,
		(BitAction)(1-GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_15)));
	}
}

/**************************(C) COPYRIGHT 2017************END OF FILE************/
