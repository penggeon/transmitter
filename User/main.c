#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "Serial.h"			// ���ڵ���
#include "LED.h"
#include "utils.h"
#include "ESP8266.h"
#include <string.h>
#include <stdlib.h>
#include "adc.h"
#include "gp2y.h"
#include "Sensor.h"
#include "mytiming.h"
#define main_program 1  // �Ƿ����������
#define use_esp8266 1   // �Ƿ����esp8266
#define use_dht11 1     // �Ƿ����DHT11
#define use_gp2y 1      // �Ƿ����GP2Y1014AU
#define use_infrared 0  // �Ƿ�������

// ---- ����ȫ�ֱ��� ----
Temperature_Typedef temp;	            // �¶Ƚṹ��
Humidity_Typedef humi;		            // ʪ�Ƚṹ��
Concentration_Typedef concentration;  // PM2.5Ũ�Ƚṹ��

ESP8266_InitTypeDef ESP8266_InitStructure; // WiFiģ���ʼ���ṹ��
ESP8266_HTTP_PostDataTypedef ESP8266_HTTP_PostDataTypestructure; // �ϴ���ʪ������API�ṹ��
ESP8266_HTTP_SendSmsTypedef ESP8266_HTTP_SendSmsTypestructure; // ���Ͷ���API�ṹ��

int16_t Num_flag;   // ���صı�־λ, 1Ϊ��λ�����Ч, 0Ϊ��λ�����Ч���λ�����Ч

uint8_t isInit = 1; // �Ƿ��ǵ�һ��
uint16_t pm;        // PM2.5Ũ��
uint8_t isSuccessInitProgram = 0;   // �Ƿ���ɳ�ʼ��
uint8_t isSuccessGetThreshold = 0;  // �Ƿ��ȡ����ֵ

uint8_t isUpdateData = 0;     // �Ƿ��ϴ�����
uint8_t isGetThreshold = 0;   // �Ƿ��ȡ��ֵ
// ---- ����ȫ�ֱ��� ----

int main(void)
{
	// ��ʼ��ǰ����NVIC�жϷ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			// ����NVICΪ����2

	// �����ʼ��
	OLED_Init();			    // OLED��ʼ��
	Serial_Init();		    // ���ڳ�ʼ��
	DH11_GPIO_Init();     // DHT11��ʼ��
	LED_Init();				    // LED��ʼ��
	ESP8266_Init();	      // WiFiģ���ʼ��
  Adc_Init();           // ADC��ʼ��
	GP2Y1014AU_init();    // PM2.5ģ���ʼ��
  Sensor_Init();		    // ��������ʼ��
  Mytiming_Init();      // ��ʱ����ʼ��

  // ---- ���ڵ��Բ��� ----
  Serial_Printf("\r\n����������!\r\n");
  // ---- ���ڵ��Բ��� ----

	// ��ʾ��ӭ��Ϣ
	Utils_ShowWelcomeMessage();

  // ---- ESP8266�����ʼ�� ----
  #if use_esp8266
  OLED_ShowString(4, 4, "ESP8266...");  // ��ʾ��ʾ��Ϣ
  ESP8266_InitStructure.ESP8266_WIFI_SSID = "bwifaa";
  ESP8266_InitStructure.ESP8266_WIFI_PASSWORD = "123456789";
  // "116.62.81.138": ��֪����������IP��ַ
  // "192.168.41.242": �������ڵĵ���TCP�ͻ���IP��ַ
  // "47.93.208.134": �����ۺ�ʵ�������IP��ַ
  // "8.130.11.139": �����ۺ�ʵ�������IP��ַ
  ESP8266_InitStructure.ESP8266_SERVER_ADDRESS = "8.130.11.139";
  ESP8266_InitStructure.ESP8266_SERVER_PORT = 80;
  uint8_t res_ESP8266Connetion = ESP8266_InitConnection(&ESP8266_InitStructure); // ���ӷ�����
  if(!res_ESP8266Connetion)
  {
    // ������TCP�ɹ����Ӻ�Ĵ���
    // ��ȡ��������
    // ESP8266_HTTP_Get("/v3/weather/now.json?key=S9BUpBsBfRyvK8ngt&location=beijing&language=zh-Hans&unit=c", 20);
    // ��ȡ��ҳ����
    // char* res = ESP8266_HTTP_Get("/index", 100);
    // ��ȡ������������
    // char* res = ESP8266_HTTP_Get("/thcp?access-token=9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa", 100);
  }
  #endif
  // ---- ESP8266�����ʼ�� ----

  // ---- DHT11 ----
  #if use_dht11
	// ��ʾ��ʪ��������
	OLED_Clear();
	OLED_ShowChinese(0,0,2);		// ��
	OLED_ShowChinese(0,16,4);		// ��
	OLED_ShowString(1, 5, ":");
	OLED_ShowString(1, 9, ".");
	OLED_ShowChinese(2,0,3);		// ʪ
	OLED_ShowChinese(2,16,4);		// ��
	OLED_ShowString(2, 5, ":");
	OLED_ShowString(2, 9, ".");
  #endif
  // ---- DHT11 ----

  // ---- GP2Y1014AU ----
  #if use_gp2y
  // ��ʾŨ�ȼ�����
  OLED_ShowString(3,1,"PM2.5");
  OLED_ShowChinese(4,40,27);		// Ũ
	OLED_ShowChinese(4,56,4);		// ��
	OLED_ShowString(3, 10, ":");
	OLED_ShowString(3, 14, ".");
  #endif
  // ---- GP2Y1014AU ----

  // ---- ���� ----
	uint8_t alarmFlag = 0; // ������־, 0:����, 1:�¶ȸ�, 2:ʪ�ȸ�, 3:PM2.5Ũ�ȸ�
	uint8_t lastAlarmFlag = 0; // ��һ�ξ�����־
  // ---- ���� ----

  isSuccessInitProgram = 1; // ��ɳ�ʼ��

	while (1)
	{
    // ---- ESP8266��ȡ��ֵ������ ----
    #if use_esp8266
    if(isGetThreshold)
    {
      char* res = ESP8266_HTTP_Get("/fixthreshold?access-token=9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa", 100);
      char* res_end;
      do
      {
        res = ESP8266_GetBuffer();
        res_end = strstr(res, "}]}");
      }while(res_end == NULL);
      char* res_json = strstr(res, "{");
      Serial_Printf("��ȡ��ֵ:\r\n%s\r\n", res_json); // ��ӡ JSON ����

      // ��ȡ�¶�, ʪ��, PM2.5Ũ��ֵ
      char* name = "temperature";
      Utils_GetInformationFromResJson_8(res_json, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
      name = "humidity";
      Utils_GetInformationFromResJson_8(res_json, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
      name = "concentration";
      Utils_GetInformationFromResJson_16(res_json, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);

      // ���±�־λ
      isSuccessGetThreshold = 1;
      isGetThreshold = 0;
    }
    #endif
    // ---- ESP8266��ȡ��ֵ������ ----

    // ---- gp2y��ȡ ----
    #if use_gp2y
    Serial_Printf("use_gp2y\r\n");
    pm = GP2Y1014AU_GetInfo();   // ��ȡŨ����Ϣ
    concentration.Concentration_H = pm;  // ��������
    concentration.Concentration_L = 0;   // С������
    #endif
    // ---- gp2y��ȡ ----

		// ---- DHT11 ----
    #if use_dht11
    Serial_Printf("use_dht11\r\n");
		DHT11ReadData(&temp.Temp_H,&temp.Temp_L,&humi.Humi_H,&humi.Humi_L); // ��ȡ����
    #endif
		// ---- DHT11 ----

    // ---- �����ж�(������) ----
    #if main_program
    Serial_Printf("main_program\r\n");
    if(isSuccessGetThreshold)
    {
      // �Ѿ���ȡ����ֵ
      // �ж��Ƿ񳬹���ֵ
      alarmFlag = Utils_IsOverThreshold(&humi,&temp,&concentration);
      Serial_Printf("alarmFlag:\r\n%d\r\n", alarmFlag); 
      if(alarmFlag == 1 && alarmFlag != lastAlarmFlag)
      {
        // �¶ȳ�����ֵ
        LED_On();
        OLED_Clear();
        OLED_ShowChinese(0,0,2);		// ��
        OLED_ShowChinese(0,16,4);		// ��
        OLED_ShowChinese(0,32,5);		// ��
        OLED_ShowChinese(0,48,6);		// ��

        // ���Ͷ���
        ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
        ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
      }
      else if(alarmFlag == 2 && alarmFlag != lastAlarmFlag)
      {
        // ʪ�ȳ�����ֵ
        LED_On();
        OLED_Clear();
        OLED_ShowChinese(0,0,3);		// ʪ
        OLED_ShowChinese(0,16,4);		// ��
        OLED_ShowChinese(0,32,5);		// ��
        OLED_ShowChinese(0,48,6);		// ��

        // ���Ͷ���
        ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
        ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
      }
      else if(alarmFlag == 3 && alarmFlag != lastAlarmFlag)
      {
        // Ũ�ȳ�����ֵ
        LED_On();
        OLED_Clear();
        OLED_ShowChinese(0,0,27);		// Ũ
        OLED_ShowChinese(0,16,4);		// ��
        OLED_ShowChinese(0,32,5);		// ��
        OLED_ShowChinese(0,48,6);   // ��

        // ���Ͷ���
        ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
        ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
      }
      else if(alarmFlag == 0)
      {
        // ����
        LED_Off();
        if(alarmFlag != lastAlarmFlag)
        {
          // ����
          OLED_Clear();
          // ��ʾ����
          OLED_ShowChinese(0,0,2);		// ��
          OLED_ShowChinese(0,16,4);		// ��
          OLED_ShowString(1, 5, ":");
          OLED_ShowString(1, 9, ".");
          OLED_ShowChinese(2,0,3);		// ʪ
          OLED_ShowChinese(2,16,4);		// ��
          OLED_ShowString(2, 5, ":");
          OLED_ShowString(2, 9, ".");
          OLED_ShowString(3,1,"PM2.5");
          OLED_ShowChinese(4,40,27);		// Ũ
          OLED_ShowChinese(4,56,4);		// ��
          OLED_ShowString(3, 10, ":");
          OLED_ShowString(3, 14, ".");
        }
        // ��ʾ�¶ȡ�ʪ�ȡ�Ũ������
        OLED_ShowNum(1,7,temp.Temp_H,2);	 	                  // �¶� ��������
        OLED_ShowNum(1,10,temp.Temp_L,1);		                  // �¶� С������
        OLED_ShowNum(2,7,humi.Humi_H,2);  	                  // ʪ�� ��������
        OLED_ShowNum(2,10,humi.Humi_L,1); 	                  // ʪ�� С������
        OLED_ShowNum(3,11,concentration.Concentration_H,3);    // Ũ�� ��������
        OLED_ShowNum(3,15,concentration.Concentration_L,1);   // Ũ�� С������
      }
      lastAlarmFlag = alarmFlag;	// ���¾�����־
    }
    else
    {
      // û�л�ȡ����ֵ
      // ��ʾ�¶ȡ�ʪ�ȡ�Ũ������
      OLED_ShowNum(1,7,temp.Temp_H,2);	 	                  // �¶� ��������
      OLED_ShowNum(1,10,temp.Temp_L,1);		                  // �¶� С������
      OLED_ShowNum(2,7,humi.Humi_H,2);  	                  // ʪ�� ��������
      OLED_ShowNum(2,10,humi.Humi_L,1); 	                  // ʪ�� С������
      OLED_ShowNum(3,11,concentration.Concentration_H,3);    // Ũ�� ��������
      OLED_ShowNum(3,15,concentration.Concentration_L,1);   // Ũ�� С������
    }
    #endif
    // ---- �����ж�(������) ----

		// ---- ESP8266�ϴ���ʪ�Ⱥ�pm2.5Ũ������ ----
    #if use_esp8266
    ESP8266_HTTP_PostDataTypestructure.temperature_H = temp.Temp_H;
    ESP8266_HTTP_PostDataTypestructure.temperature_L = temp.Temp_L;
    ESP8266_HTTP_PostDataTypestructure.humidity_H = humi.Humi_H;
    ESP8266_HTTP_PostDataTypestructure.humidity_L = humi.Humi_L;
    ESP8266_HTTP_PostDataTypestructure.concentration_H = pm;
    ESP8266_HTTP_PostDataTypestructure.concentration_L = 0;
    ESP8266_HTTP_PostDataTypestructure.timeout = 20;
    if(isUpdateData)
    {
      char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // �ϴ���������
      char* res_end;
      do
      {
        res = ESP8266_GetBuffer();
        res_end = strstr(res, "}");
      }while(res_end == NULL);
      char* res_json = strstr(res, "{");
      Serial_Printf("�ϴ�����������Ӧ:\r\n%s\r\n", res_json); // ��ӡ JSON ����
    }
    #endif
    // ---- ESP8266�ϴ���ʪ�Ⱥ�pm2.5Ũ������ ----

    // ---- ���� ----
    #if use_infrared
    Num_flag = Sensor_Get();
		OLED_ShowSignedNum(1, 7, Num_flag, 1);	//��ʾNum
    #endif
    // ---- ���� ----

		// ---- ��ʱ ----
		Delay_ms(1000);
		// ---- ��ʱ ----
	}
}

void Mytiming_callback_1s(void)
{
  isUpdateData = 1;
  #if 0
	// ---- ESP8266��ȡ��ֵ������ ----
  if(1)
  {
    char* res = ESP8266_HTTP_Get("/fixthreshold?access-token=9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa", 100);
    char* res_end;
    do
    {
      res = ESP8266_GetBuffer();
      res_end = strstr(res, "}]}");
    }while(res_end == NULL);
    char* res_json = strstr(res, "{");
    Serial_Printf("��ȡ��ֵ:\r\n%s\r\n", res_json); // ��ӡ JSON ����

    // ��ȡ�¶�, ʪ��, PM2.5Ũ��ֵ
    char* name = "temperature";
    Utils_GetInformationFromResJson_8(res_json, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
    name = "humidity";
    Utils_GetInformationFromResJson_8(res_json, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
    name = "concentration";
    Utils_GetInformationFromResJson_16(res_json, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);
    isSuccessGetThreshold = 1;
  }
	// ---- ESP8266��ȡ��ֵ������ ----

  // ---- ESP8266�ϴ���ʪ�Ⱥ�pm2.5Ũ������ ----
  ESP8266_HTTP_PostDataTypestructure.temperature_H = temp.Temp_H;
  ESP8266_HTTP_PostDataTypestructure.temperature_L = temp.Temp_L;
  ESP8266_HTTP_PostDataTypestructure.humidity_H = humi.Humi_H;
  ESP8266_HTTP_PostDataTypestructure.humidity_L = humi.Humi_L;
  ESP8266_HTTP_PostDataTypestructure.concentration_H = pm;
  ESP8266_HTTP_PostDataTypestructure.concentration_L = 0;
  ESP8266_HTTP_PostDataTypestructure.timeout = 20;
  if(1)
  {
    char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // �ϴ���������
    char* res_end;
    do
    {
      res = ESP8266_GetBuffer();
      res_end = strstr(res, "}");
    }while(res_end == NULL);
    char* res_json = strstr(res, "{");
    Serial_Printf("�ϴ�����������Ӧ:\r\n%s\r\n", res_json); // ��ӡ JSON ����
  }
  // ---- ESP8266�ϴ���ʪ�Ⱥ�pm2.5Ũ������ ----
  #endif
}

void Mytiming_callback_5s(void)
{
  isGetThreshold = 1;
  #if 0
	// ---- ESP8266��ȡ��ֵ������ ----
  if(1)
  {
    char* res = ESP8266_HTTP_Get("/fixthreshold?access-token=9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa", 100);
    char* res_end;
    do
    {
      res = ESP8266_GetBuffer();
      res_end = strstr(res, "}]}");
    }while(res_end == NULL);
    char* res_json = strstr(res, "{");
    Serial_Printf("��ȡ��ֵ:\r\n%s\r\n", res_json); // ��ӡ JSON ����

    // ��ȡ�¶�, ʪ��, PM2.5Ũ��ֵ
    char* name = "temperature";
    Utils_GetInformationFromResJson_8(res_json, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
    name = "humidity";
    Utils_GetInformationFromResJson_8(res_json, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
    name = "concentration";
    Utils_GetInformationFromResJson_16(res_json, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);
    isSuccessGetThreshold = 1;
  }
	// ---- ESP8266��ȡ��ֵ������ ----

  // ---- ESP8266�ϴ���ʪ�Ⱥ�pm2.5Ũ������ ----
  ESP8266_HTTP_PostDataTypestructure.temperature_H = temp.Temp_H;
  ESP8266_HTTP_PostDataTypestructure.temperature_L = temp.Temp_L;
  ESP8266_HTTP_PostDataTypestructure.humidity_H = humi.Humi_H;
  ESP8266_HTTP_PostDataTypestructure.humidity_L = humi.Humi_L;
  ESP8266_HTTP_PostDataTypestructure.concentration_H = pm;
  ESP8266_HTTP_PostDataTypestructure.concentration_L = 0;
  ESP8266_HTTP_PostDataTypestructure.timeout = 20;
  if(1)
  {
    char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // �ϴ���������
    char* res_end;
    do
    {
      res = ESP8266_GetBuffer();
      res_end = strstr(res, "}");
    }while(res_end == NULL);
    char* res_json = strstr(res, "{");
    Serial_Printf("�ϴ�����������Ӧ:\r\n%s\r\n", res_json); // ��ӡ JSON ����
  }
  // ---- ESP8266�ϴ���ʪ�Ⱥ�pm2.5Ũ������ ----
  #endif
}

/**
 * @brief  TIM2�жϺ���������, 1ms����һ��
 * @param  ��
 * @retval ��
*/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
    if(isSuccessInitProgram)
    {
      if(Mytiming_CountGet() == 4999)
      {
        Mytiming_callback_5s();
        Mytiming_callback_1s();
        // Serial_Printf("Mytiming_CountGet:\r\n%d\r\n", Mytiming_CountGet());
        Mytiming_CountReset();
      }
      if(Mytiming_CountGet() == 999 || Mytiming_CountGet() == 1999 || Mytiming_CountGet() == 2999 || Mytiming_CountGet() == 3999)
      {
        Mytiming_callback_1s();
        // Serial_Printf("Mytiming_CountGet:\r\n%d\r\n", Mytiming_CountGet());
        Mytiming_CountAdd();
      }
      else
      {
        Mytiming_CountAdd();
      }
    }
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
