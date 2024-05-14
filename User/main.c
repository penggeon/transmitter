#include "stm32f10x.h"                  // Device header
#include <string.h>
#include <stdlib.h>
#include "Delay.h"                      // ��ʱ����
#include "Serial.h"                     // ����
#include "utils.h"                      // ���ߺ���
#include "mytiming.h"                   // ��ʱ��
#include "OLED.h"                       // ģ�� - OLED
#include "DHT11.h"                      // ģ�� - DHT11
#include "LED.h"                        // ģ�� - LED
#include "ESP8266.h"                    // ģ�� - ESP8266
#include "gp2y.h"                       // ģ�� - GP2Y1014AU
#include "Sensor.h"                     // ģ�� - ���⴫����
#include "Motor.h"                      // ģ�� - ���

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
uint16_t pm;        // PM2.5Ũ��
int8_t Speed;       // ���ת��(-100 ~ +100)

// ��ʶ
uint8_t mainProgramSuccessInit = 0;       // ��������Ƿ���ɳ�ʼ��
uint8_t ESP8266GetThresholdSuccess = 0;   // ESP8266�Ƿ��ȡ����ֵ(״̬)
uint8_t ESP8266GetThreshold = 0;          // ESP8266�Ƿ��ȡ��ֵ(����)
uint8_t ESP8266UpdateData = 0;            // ESP8266�Ƿ��ϴ�����(����)
// ---- ����ȫ�ֱ��� ----

int main(void)
{
	// ��ʼ��ǰ����NVIC�жϷ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			// ����NVICΪ����2

	// �����ʼ��
	Serial_Init();		    // ���ڳ�ʼ��
  Mytiming_Init();      // ��ʱ����ʼ��
	OLED_Init();			    // ģ�� - OLED��ʼ��
	DH11_GPIO_Init();     // ģ�� - DHT11��ʼ��
	LED_Init();				    // ģ�� - LED��ʼ��
	ESP8266_Init();	      // ģ�� - ESP8266��ʼ��
	GP2Y1014AU_init();    // ģ�� - GP2Y1014AU��ʼ��
  Sensor_Init();		    // ģ�� - ���⴫������ʼ��
  Motor_Init();		      // ģ�� - �����ʼ��

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
  ESP8266_InitStructure.ESP8266_SERVER_ADDRESS = "8.130.11.139";
                                        // "116.62.81.138": ��֪����������IP��ַ
                                        // "192.168.41.242": �������ڵĵ���TCP�ͻ���IP��ַ
                                        // "47.93.208.134": �����ۺ�ʵ�������IP��ַ
                                        // "8.130.11.139": �����ۺ�ʵ�������IP��ַ
  ESP8266_InitStructure.ESP8266_SERVER_PORT = 8080;
  uint8_t res_ESP8266Connetion = ESP8266_InitConnection(&ESP8266_InitStructure); // ���ӷ�����
  if(!res_ESP8266Connetion)
  {
  	OLED_Clear();   // ���ӳɹ�, ���OLED
  }
  #endif
  // ---- ESP8266�����ʼ�� ----

  // ---- DHT11 ----
  #if use_dht11
	// ��ʾ��ʪ��������
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

  mainProgramSuccessInit = 1; // ��ɳ�ʼ��

	while (1)
	{
    // ---- ESP8266��ȡ��ֵ������ ----
    #if use_esp8266
    if(ESP8266GetThreshold)
    {
      Serial_Printf("ESP8266 ��ȡ��ֵ.\r\n");
      char* res = ESP8266_HTTP_Get("/fixthreshold?access=9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa", 100);
      char* res_end;
      do
      {
        res = ESP8266_GetBuffer();
        // Serial_Printf("res: %s\r\n", res);
        res_end = strstr(res, "}]}");
        Delay_ms(100);
      }while(res_end == NULL);
      // Serial_Printf("res: %s\r\n", res);

      // ��ȡ�¶�, ʪ��, PM2.5Ũ��ֵ
      char* name = "temperature";
      Utils_GetInformationOfThreshold_8(res, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
      name = "humidity";
      Utils_GetInformationOfThreshold_8(res, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
      name = "concentration";
      Utils_GetInformationOfThreshold_16(res, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);
      // Serial_Printf("�����걾������\r\n");
      // Serial_Printf("temp*:%d.%d\r\n", temp.Temp_H_Threshold, temp.Temp_L_Threshold);
      // Serial_Printf("humi*:%d.%d\r\n", humi.Humi_H_Threshold, humi.Humi_L_Threshold);
      // Serial_Printf("concentration*:%d.%d\r\n", concentration.Concentration_H_Threshold, concentration.Concentration_L_Threshold);

      // ���±�־λ
      ESP8266GetThresholdSuccess = 1;
      ESP8266GetThreshold = 0;
    }
    #endif
    // ---- ESP8266��ȡ��ֵ������ ----

    // ---- gp2y��ȡ ----
    #if use_gp2y
    // Serial_Printf("gp2y\r\n");
    pm = GP2Y1014AU_GetInfo();   // ��ȡŨ����Ϣ
    concentration.Concentration_H = pm;  // ��������
    concentration.Concentration_L = 0;   // С������
    #endif
    // ---- gp2y��ȡ ----

		// ---- DHT11 ----
    #if use_dht11
    // Serial_Printf("dht11\r\n");
		DHT11ReadData(&temp.Temp_H,&temp.Temp_L,&humi.Humi_H,&humi.Humi_L); // ��ȡ����
    #endif
		// ---- DHT11 ----

    // ---- �����ж�(������) ----
    #if main_program
    // Serial_Printf("main_program\r\n");
    if(ESP8266GetThresholdSuccess)
    {
      // �Ѿ���ȡ����ֵ
      // �ж��Ƿ񳬹���ֵ
      alarmFlag = Utils_IsOverThreshold(&humi,&temp,&concentration);
      Serial_Printf("�¶�����: %d, %d, %d, %d\r\n", temp.Temp_H, temp.Temp_L, temp.Temp_H_Threshold, temp.Temp_L_Threshold);
      Serial_Printf("alarmFlag: %d\r\n", alarmFlag);
      if(alarmFlag == 1)
      {
        // ��ǰ�¶ȳ�����ֵ
        if(alarmFlag != lastAlarmFlag)
        {
          // �����ж�Ϊ�¶ȹ���, ����������ֻ��ִ��һ��
          // LED_On();
          OLED_Clear();
          // �¶ȹ���
          OLED_ShowChinese(0,0,2);		// ��
          OLED_ShowChinese(0,16,4);		// ��
          OLED_ShowChinese(0,32,5);		// ��
          OLED_ShowChinese(0,48,6);		// ��

          // ���Ͷ���
          // ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
          // ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
          Serial_Printf("�¶ȹ���\r\n");
        }
        // ��ʾ�����¶�ֵ
        OLED_ShowChinese(2,0,2);		// ��
        OLED_ShowChinese(2,16,4);		// ��
        OLED_ShowString(2, 5, ":");
        OLED_ShowString(2, 9, ".");
        OLED_ShowNum(2,7,temp.Temp_H,2);    // �¶� ��������
        OLED_ShowNum(2,10,temp.Temp_L,1);   // �¶� С������

        // ����ת��
        Speed = Utils_GetPIDOutputSpeed(temp.Temp_H, temp.Temp_L, temp.Temp_H_Threshold, temp.Temp_L_Threshold);
        Motor_SetSpeed(Speed);
      }
      else if(alarmFlag == 2)
      {
        // ��ǰʪ�ȳ�����ֵ
        if(alarmFlag != lastAlarmFlag)
        {
          // LED_On();
          OLED_Clear();
          // ʪ�ȹ���
          OLED_ShowChinese(0,0,3);		// ʪ
          OLED_ShowChinese(0,16,4);		// ��
          OLED_ShowChinese(0,32,5);		// ��
          OLED_ShowChinese(0,48,6);		// ��

          // ���Ͷ���
          // ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
          // ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
          Serial_Printf("ʪ�ȹ���\r\n");
        }
        // ��ʾ����ʪ��ֵ
        OLED_ShowChinese(2,0,3);		// ʪ
        OLED_ShowChinese(2,16,4);		// ��
        OLED_ShowString(2, 5, ":");
        OLED_ShowString(2, 9, ".");
        OLED_ShowNum(2,7,humi.Humi_H,2);    // ʪ�� ��������
        OLED_ShowNum(2,10,humi.Humi_L,1);   // ʪ�� С������
      }
      else if(alarmFlag == 3)
      {
        // ��ǰŨ�ȳ�����ֵ
        if(alarmFlag != lastAlarmFlag)
        {
          // LED_On();
          OLED_Clear();
          // Ũ�ȹ���
          OLED_ShowChinese(0,0,27);		// Ũ
          OLED_ShowChinese(0,16,4);		// ��
          OLED_ShowChinese(0,32,5);		// ��
          OLED_ShowChinese(0,48,6);   // ��

          // ���Ͷ���
          // ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
          // ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
          Serial_Printf("Ũ�ȹ���\r\n");
        }
        // ��ʾ����Ũ��ֵ
        OLED_ShowString(2,1,"PM2.5");
        OLED_ShowChinese(2,40,27);	// Ũ
        OLED_ShowChinese(2,56,4);		// ��
        OLED_ShowString(2, 10, ":");
        OLED_ShowString(2, 14, ".");
        OLED_ShowNum(2,11,concentration.Concentration_H,3);   // Ũ�� ��������
        OLED_ShowNum(2,15,concentration.Concentration_L,1);   // Ũ�� С������
      }
      else if(alarmFlag == 0)
      {
        // ��ǰ����
        LED_Off();
        if(alarmFlag != lastAlarmFlag)
        {
          // ����
          OLED_Clear();
          // ��ʾ����
          OLED_ShowChinese(0,0,2);		  // ��
          OLED_ShowChinese(0,16,4);		  // ��
          OLED_ShowString(1, 5, ":");
          OLED_ShowString(1, 9, ".");
          OLED_ShowChinese(2,0,3);		  // ʪ
          OLED_ShowChinese(2,16,4);		  // ��
          OLED_ShowString(2, 5, ":");
          OLED_ShowString(2, 9, ".");
          OLED_ShowString(3,1,"PM2.5");
          OLED_ShowChinese(4,40,27);		// Ũ
          OLED_ShowChinese(4,56,4);		  // ��
          OLED_ShowString(3, 10, ":");
          OLED_ShowString(3, 14, ".");
          Serial_Printf("��ʪŨ������.\r\n");
        }
        // ��ʾ�¶ȡ�ʪ�ȡ�Ũ������
        OLED_ShowNum(1,7,temp.Temp_H,2);	 	                  // �¶� ��������
        OLED_ShowNum(1,10,temp.Temp_L,1);		                  // �¶� С������
        OLED_ShowNum(2,7,humi.Humi_H,2);  	                  // ʪ�� ��������
        OLED_ShowNum(2,10,humi.Humi_L,1); 	                  // ʪ�� С������
        OLED_ShowNum(3,11,concentration.Concentration_H,3);   // Ũ�� ��������
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
      OLED_ShowNum(3,11,concentration.Concentration_H,3);   // Ũ�� ��������
      OLED_ShowNum(3,15,concentration.Concentration_L,1);   // Ũ�� С������
    }
    #endif
    // ---- �����ж�(������) ----

		// ---- ESP8266�ϴ�����Ũ������ ----
    #if use_esp8266
    ESP8266_HTTP_PostDataTypestructure.temperature_H = temp.Temp_H;
    ESP8266_HTTP_PostDataTypestructure.temperature_L = temp.Temp_L;
    ESP8266_HTTP_PostDataTypestructure.humidity_H = humi.Humi_H;
    ESP8266_HTTP_PostDataTypestructure.humidity_L = humi.Humi_L;
    ESP8266_HTTP_PostDataTypestructure.concentration_H = pm;
    ESP8266_HTTP_PostDataTypestructure.concentration_L = 0;
    ESP8266_HTTP_PostDataTypestructure.timeout = 20;
    if(ESP8266UpdateData)
    {
      Serial_Printf("ESP8266 �ϴ���ʪŨ������.\r\n");
      char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // �ϴ���������
      char* res_end;
      do
      {
        res = ESP8266_GetBuffer();
        res_end = strchr(res, '}'); 
        // Serial_Printf("res: %s\r\n", res);
        Delay_ms(100);
      }while(res_end == NULL);
      // res = strchr(res, '{');
      // Serial_Printf("res: %s\r\n", res);

      // ���±�־λ
      ESP8266UpdateData = 0;
    }
    #endif
    // ---- ESP8266�ϴ�����Ũ������ ----

    // ---- ���� ----
    #if use_infrared
    Num_flag = Sensor_Get();
		OLED_ShowSignedNum(1, 7, Num_flag, 1);	//��ʾNum
    #endif
    // ---- ���� ----

		// ---- ��ʱ ----
		// Delay_ms(1000);
		// ---- ��ʱ ----
	}
}

// ---- ��ʱ ----

void Mytiming_callback_1s(void)
{

}

void Mytiming_callback_3s(void)
{
  ESP8266UpdateData = 1;    // �ϴ�����Ũ������
}

void Mytiming_callback_5s(void)
{
  ESP8266GetThreshold = 1;  // ��ȡ��ֵ
}

// ��ʱ�ü�����
uint32_t Mytiming_count_1s = 0;
uint32_t Mytiming_count_3s = 0;
uint32_t Mytiming_count_5s = 0;

/**
 * @brief  TIM2�жϺ���������, 1ms����һ��
 * @param  ��
 * @retval ��
*/
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
  {
    if(mainProgramSuccessInit)
    {
      if(Mytiming_count_1s>=999)
      {
        Mytiming_callback_1s();
        Mytiming_count_1s = 0;
      }
      if(Mytiming_count_3s>=2999)
      {
        Mytiming_callback_3s();
        Mytiming_count_3s = 0;
      }
      if(Mytiming_count_5s>=4999)
      {
        Mytiming_callback_5s();
        Mytiming_count_5s = 0;
      }

      // ����������
      Mytiming_count_1s++;
      Mytiming_count_3s++;
      Mytiming_count_5s++;
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}
// ---- ��ʱ ----
