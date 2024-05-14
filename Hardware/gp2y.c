#include "gp2y.h"

/**
 * @brief  GP2Y1014AU��ʼ��
 * @param  None
 * @retval None
 */
void GP2Y1014AU_init(void){
  // ��ʼ��GPIO GP2Y1014AU��������
  GPIO_InitTypeDef GPIO_InitStruce;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ����GPIOAʱ��
  GPIO_InitStruce.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruce.GPIO_Pin = gp2y_GPIO_PIN;
  GPIO_InitStruce.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(gp2y_GPIO, &GPIO_InitStruce);
}

/**
 * @brief  GP2Y1014AU��ȡPM2.5Ũ��
 * @param  None
 * @retval ����ֵ
 */
uint16_t GP2Y1014AU_GetInfo(void)
{
  uint16_t AD_PM = 0;
  gp2y_off;   // ���͵�ƽ
  Delay_us(280);
  AD_PM=Get_Adc_Average(1,10);      // �ɼ�ADCֵ
  Delay_us(40);
  gp2y_on;   // ���ߵ�ƽ
  Delay_us(9680);
  // return AD_PM;
  float Voltage = 3.3f * AD_PM / 255.f * 2;    // ���AO����ڵĵ�ѹֵ
  // float Voltage = 3.3f * AD_PM / 4096.f * 2;    // ���AO����ڵĵ�ѹֵ
	int dustVal = (0.17*Voltage-0.1)*1000;   // ����1000��λ����ug/m3
  
  // ��λ
  if(dustVal < 0)
  {
		dustVal = 0;
  }
	if(dustVal > 500)
  {
		dustVal = 500;
  }
	// Serial_Printf("\r\nAD_PM:%d\r\n",AD_PM);
	// Serial_Printf("\r\n��ѹֵ:%fV\r\n",Voltage);
	// Serial_Printf("\r\n�ҳ�Ũ��:%d ug/m3\r\n",dustVal);

	return dustVal;
}
