#ifndef __LED_H__
#define __LED_H__

#define LED_GPIO_PORT GPIOB
#define LED_GPIO_PIN GPIO_Pin_12

void LED_Init(void);
void LED_On(void);
void LED_Off(void);

#endif
