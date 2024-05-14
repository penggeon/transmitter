#include "gp2y.h"

/**
 * @brief  GP2Y1014AU初始化
 * @param  None
 * @retval None
 */
void GP2Y1014AU_init(void){
  // 初始化GPIO GP2Y1014AU控制引脚
  GPIO_InitTypeDef GPIO_InitStruce;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启GPIOA时钟
  GPIO_InitStruce.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruce.GPIO_Pin = gp2y_GPIO_PIN;
  GPIO_InitStruce.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(gp2y_GPIO, &GPIO_InitStruce);
}

/**
 * @brief  GP2Y1014AU获取PM2.5浓度
 * @param  None
 * @retval 距离值
 */
uint16_t GP2Y1014AU_GetInfo(void)
{
  uint16_t AD_PM = 0;
  gp2y_off;   // 拉低电平
  Delay_us(280);
  AD_PM=Get_Adc_Average(1,10);      // 采集ADC值
  Delay_us(40);
  gp2y_on;   // 拉高电平
  Delay_us(9680);
  // return AD_PM;
  float Voltage = 3.3f * AD_PM / 255.f * 2;    // 获得AO输出口的电压值
  // float Voltage = 3.3f * AD_PM / 4096.f * 2;    // 获得AO输出口的电压值
	int dustVal = (0.17*Voltage-0.1)*1000;   // 乘以1000单位换成ug/m3
  
  // 限位
  if(dustVal < 0)
  {
		dustVal = 0;
  }
	if(dustVal > 500)
  {
		dustVal = 500;
  }
	// Serial_Printf("\r\nAD_PM:%d\r\n",AD_PM);
	// Serial_Printf("\r\n电压值:%fV\r\n",Voltage);
	// Serial_Printf("\r\n灰尘浓度:%d ug/m3\r\n",dustVal);

	return dustVal;
}
