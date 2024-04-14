#include "stm32f10x.h"
#include "LED.h"

/** 
  * @brief  LED初始化
  * @param  无
  * @retval 无
  */
void LED_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

  LED_Off();
}

void LED_On(void)
{
  GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}

void LED_Off(void)
{
  GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}
