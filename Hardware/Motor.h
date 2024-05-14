#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"
#include "PWM.h"

#define AIN1_GPIO_Port GPIOB
#define AIN1_GPIO_Pin GPIO_Pin_15
#define AIN2_GPIO_Port GPIOB
#define AIN2_GPIO_Pin GPIO_Pin_14

void Motor_Init(void);
void Motor_SetSpeed(int8_t Speed);

#endif
