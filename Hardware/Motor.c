#include "Motor.h"

/**
 * @brief  电机初始化
 * @param  None
 * @retval None
 */
void Motor_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);         // GPIOB时钟使能
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = AIN1_GPIO_Pin | AIN2_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                        // PB14, PB15设置为推挽输出
	
	PWM_Init();                                                   // PWM初始化
}

/**
 * @brief  设置电机速度
 * @param  Speed: 速度值
 * @retval None
 */
void Motor_SetSpeed(int8_t Speed)
{
	if(Speed >= 0)
	{
    // 正转
		GPIO_SetBits(AIN1_GPIO_Port, AIN1_GPIO_Pin);
		GPIO_ResetBits(AIN2_GPIO_Port, AIN2_GPIO_Pin);
		PWM_SetCompare3(Speed);
	}
	else
	{
    // 反转
		GPIO_ResetBits(AIN1_GPIO_Port, AIN1_GPIO_Pin);
		GPIO_SetBits(AIN2_GPIO_Port, AIN2_GPIO_Pin);
		PWM_SetCompare3(-Speed);
	}
}
