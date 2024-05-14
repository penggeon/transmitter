#ifndef __GP2Y_H
#define __GP2Y_H

#include "stm32f10x.h"
#include "Delay.h"
#include "adc.h"
#include "Serial.h"

#define gp2y_GPIO GPIOA
#define gp2y_GPIO_PIN GPIO_Pin_6
#define gp2y_on GPIO_SetBits(GPIOA,gp2y_GPIO_PIN)
#define gp2y_off GPIO_ResetBits(GPIOA,gp2y_GPIO_PIN)

void GP2Y1014AU_init(void);
uint16_t GP2Y1014AU_GetInfo(void);

#endif
