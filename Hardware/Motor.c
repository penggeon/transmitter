#include "Motor.h"

/**
 * @brief  �����ʼ��
 * @param  None
 * @retval None
 */
void Motor_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);         // GPIOBʱ��ʹ��
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = AIN1_GPIO_Pin | AIN2_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                        // PB14, PB15����Ϊ�������
	
	PWM_Init();                                                   // PWM��ʼ��
}

/**
 * @brief  ���õ���ٶ�
 * @param  Speed: �ٶ�ֵ
 * @retval None
 */
void Motor_SetSpeed(int8_t Speed)
{
	if(Speed >= 0)
	{
    // ��ת
		GPIO_SetBits(AIN1_GPIO_Port, AIN1_GPIO_Pin);
		GPIO_ResetBits(AIN2_GPIO_Port, AIN2_GPIO_Pin);
		PWM_SetCompare3(Speed);
	}
	else
	{
    // ��ת
		GPIO_ResetBits(AIN1_GPIO_Port, AIN1_GPIO_Pin);
		GPIO_SetBits(AIN2_GPIO_Port, AIN2_GPIO_Pin);
		PWM_SetCompare3(-Speed);
	}
}
