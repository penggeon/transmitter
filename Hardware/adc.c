#include "stm32f10x.h"
#include "Delay.h"

/**
 * @brief  ADC��ʼ��, ʹ��ADC1_IN1 - PA1
 * @retval None
 */
void  Adc_Init(void)		//A1���ܽ��
{   
  ADC_InitTypeDef ADC_InitStructure;    // ADC��ʼ���ṹ��
  GPIO_InitTypeDef GPIO_InitStructure;  // GPIO��ʼ���ṹ��

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);  // ����ʱ��
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADCʱ��

  // ��ʼ��PA1Ϊain
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  ADC_DeInit(ADC1);  

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;   
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure); 

  ADC_Cmd(ADC1, ENABLE);  
  ADC_ResetCalibration(ADC1); 
  while(ADC_GetResetCalibrationStatus(ADC1)); 
  ADC_StartCalibration(ADC1);  
  while(ADC_GetCalibrationStatus(ADC1));  
}

/**
 * @brief  ��ȡADCֵ
 * @param  ch:ͨ����
 * @retval ADCֵ
 */
uint16_t Get_Adc(uint8_t ch)
{
  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);     

  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
  return ADC_GetConversionValue(ADC1);
}

/**
 * @brief  ��ȡADCֵ(ƽ��ֵ)
 * @param  ch:ͨ����
 * @param  times:����
 * @retval ADCֵ
 */
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
  uint32_t temp_val=0;
  uint8_t t;
  // �ۼ�ֵ
  for(t=0;t<times;t++)
  {
    temp_val+=Get_Adc(ch);
    Delay_ms(5);
  }
  return temp_val/times;
}    
