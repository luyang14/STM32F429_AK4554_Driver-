/********************     (C) COPYRIGHT 2017     **************************
 * 文件名  ：Tim2.c
 * 描述    ：定时器timer2 测试例程       
 * 实验平台：STM32F429ZGT6
 * 库版本  ：V1.6.1
 *
 * 编写日期：2017-02-07
 * 修改日期：
 * 作者    :
****************************************************************************/
#include "stm32f4_Tim2.h"

/**********************************
 * 函数名：TIM2_NVIC_Configuration
 * 描述  ：TIM2中断优先级配置
 * 输入  ：无
 * 输出  ：无	
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
 * 函数名：TIM2_Configuration
 * 描述  ：TIM_Period--1000   TIM_Prescaler--(22500-1) -->中断周期为
 * ((1/180000000)*8)*22500*1000=1S    1秒定时器
 * 输入  ：无
 * 输出  ：无	
 ********************************************************************/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=1000;		 // 自动重装载寄存器周期的值(计数值) 
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (22500 - 1);	// 时钟预分频数 (180M/8)/22500 
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		// 采样分频 
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							// 清除溢出中断标志 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);														// 开启时钟 
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);		// 先关闭等待使用
}
/**************************
 * 函数名：TIM2_Config
 * 描述  ：TIM2配置
 * 输入  ：无
 * 输出  ：无	
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
