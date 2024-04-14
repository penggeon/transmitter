#include "stm32f10x.h"
#include "mytiming.h"
uint32_t Mytiming_Count;

/**
 * @brief  TIM2 初始化
 * @param  无
 * @retval 无
 */
void Mytiming_Init(void)
{
	/* RCC 开启时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	/* 选择时基单元的时钟源，内部时钟源 */
	TIM_InternalClockConfig(TIM2);
	
	/* 配置时基单元 */
  // 初始72MHz
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		// 使能中断
		
	/* 配置 NVIC，配置优先级 */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 启动计时器 */
	TIM_Cmd(TIM2, ENABLE);
}

uint32_t Mytiming_CountGet(void)
{
	return Mytiming_Count;
}

void Mytiming_CountAdd(void)
{
	Mytiming_Count++;
}

void Mytiming_CountReset(void)
{
	Mytiming_Count = 0;
}
