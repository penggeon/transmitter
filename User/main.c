#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "Serial.h"			// 串口调试
#include "LED.h"
#include "utils.h"
#include "ESP8266.h"
#include <string.h>
#include <stdlib.h>
#include "adc.h"
#include "gp2y.h"
#include "Sensor.h"
#include "mytiming.h"
#define main_program 1  // 是否编译主程序
#define use_esp8266 1   // 是否编译esp8266
#define use_dht11 1     // 是否编译DHT11
#define use_gp2y 1      // 是否编译GP2Y1014AU
#define use_infrared 0  // 是否编译红外

// ---- 定义全局变量 ----
Temperature_Typedef temp;	            // 温度结构体
Humidity_Typedef humi;		            // 湿度结构体
Concentration_Typedef concentration;  // PM2.5浓度结构体

ESP8266_InitTypeDef ESP8266_InitStructure; // WiFi模块初始化结构体
ESP8266_HTTP_PostDataTypedef ESP8266_HTTP_PostDataTypestructure; // 上传温湿度数据API结构体
ESP8266_HTTP_SendSmsTypedef ESP8266_HTTP_SendSmsTypestructure; // 发送短信API结构体

int16_t Num_flag;   // 返回的标志位, 1为低位检测有效, 0为高位检测有效或低位检测无效

uint8_t isInit = 1; // 是否是第一次
uint16_t pm;        // PM2.5浓度
uint8_t isSuccessInitProgram = 0;   // 是否完成初始化
uint8_t isSuccessGetThreshold = 0;  // 是否获取到阈值

uint8_t isUpdateData = 0;     // 是否上传数据
uint8_t isGetThreshold = 0;   // 是否获取阈值
// ---- 定义全局变量 ----

int main(void)
{
	// 初始化前配置NVIC中断分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			// 配置NVIC为分组2

	// 外设初始化
	OLED_Init();			    // OLED初始化
	Serial_Init();		    // 串口初始化
	DH11_GPIO_Init();     // DHT11初始化
	LED_Init();				    // LED初始化
	ESP8266_Init();	      // WiFi模块初始化
  Adc_Init();           // ADC初始化
	GP2Y1014AU_init();    // PM2.5模块初始化
  Sensor_Init();		    // 传感器初始化
  Mytiming_Init();      // 定时器初始化

  // ---- 串口调试测试 ----
  Serial_Printf("\r\n程序已启动!\r\n");
  // ---- 串口调试测试 ----

	// 显示欢迎信息
	Utils_ShowWelcomeMessage();

  // ---- ESP8266软件初始化 ----
  #if use_esp8266
  OLED_ShowString(4, 4, "ESP8266...");  // 显示提示信息
  ESP8266_InitStructure.ESP8266_WIFI_SSID = "bwifaa";
  ESP8266_InitStructure.ESP8266_WIFI_PASSWORD = "123456789";
  // "116.62.81.138": 心知天气服务器IP地址
  // "192.168.41.242": 局域网内的电脑TCP客户端IP地址
  // "47.93.208.134": 测试综合实验服务器IP地址
  // "8.130.11.139": 最终综合实验服务器IP地址
  ESP8266_InitStructure.ESP8266_SERVER_ADDRESS = "8.130.11.139";
  ESP8266_InitStructure.ESP8266_SERVER_PORT = 80;
  uint8_t res_ESP8266Connetion = ESP8266_InitConnection(&ESP8266_InitStructure); // 连接服务器
  if(!res_ESP8266Connetion)
  {
    // 以下是TCP成功连接后的代码
    // 获取天气数据
    // ESP8266_HTTP_Get("/v3/weather/now.json?key=S9BUpBsBfRyvK8ngt&location=beijing&language=zh-Hans&unit=c", 20);
    // 获取首页数据
    // char* res = ESP8266_HTTP_Get("/index", 100);
    // 获取机房环境数据
    // char* res = ESP8266_HTTP_Get("/thcp?access-token=9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa", 100);
  }
  #endif
  // ---- ESP8266软件初始化 ----

  // ---- DHT11 ----
  #if use_dht11
	// 显示温湿度三个字
	OLED_Clear();
	OLED_ShowChinese(0,0,2);		// 温
	OLED_ShowChinese(0,16,4);		// 度
	OLED_ShowString(1, 5, ":");
	OLED_ShowString(1, 9, ".");
	OLED_ShowChinese(2,0,3);		// 湿
	OLED_ShowChinese(2,16,4);		// 度
	OLED_ShowString(2, 5, ":");
	OLED_ShowString(2, 9, ".");
  #endif
  // ---- DHT11 ----

  // ---- GP2Y1014AU ----
  #if use_gp2y
  // 显示浓度几个字
  OLED_ShowString(3,1,"PM2.5");
  OLED_ShowChinese(4,40,27);		// 浓
	OLED_ShowChinese(4,56,4);		// 度
	OLED_ShowString(3, 10, ":");
	OLED_ShowString(3, 14, ".");
  #endif
  // ---- GP2Y1014AU ----

  // ---- 其他 ----
	uint8_t alarmFlag = 0; // 警报标志, 0:正常, 1:温度高, 2:湿度高, 3:PM2.5浓度高
	uint8_t lastAlarmFlag = 0; // 上一次警报标志
  // ---- 其他 ----

  isSuccessInitProgram = 1; // 完成初始化

	while (1)
	{
    // ---- ESP8266获取阈值并更新 ----
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
      Serial_Printf("获取阈值:\r\n%s\r\n", res_json); // 打印 JSON 数据

      // 获取温度, 湿度, PM2.5浓度值
      char* name = "temperature";
      Utils_GetInformationFromResJson_8(res_json, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
      name = "humidity";
      Utils_GetInformationFromResJson_8(res_json, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
      name = "concentration";
      Utils_GetInformationFromResJson_16(res_json, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);

      // 更新标志位
      isSuccessGetThreshold = 1;
      isGetThreshold = 0;
    }
    #endif
    // ---- ESP8266获取阈值并更新 ----

    // ---- gp2y获取 ----
    #if use_gp2y
    Serial_Printf("use_gp2y\r\n");
    pm = GP2Y1014AU_GetInfo();   // 获取浓度信息
    concentration.Concentration_H = pm;  // 整数部分
    concentration.Concentration_L = 0;   // 小数部分
    #endif
    // ---- gp2y获取 ----

		// ---- DHT11 ----
    #if use_dht11
    Serial_Printf("use_dht11\r\n");
		DHT11ReadData(&temp.Temp_H,&temp.Temp_L,&humi.Humi_H,&humi.Humi_L); // 获取数据
    #endif
		// ---- DHT11 ----

    // ---- 本地判断(主程序) ----
    #if main_program
    Serial_Printf("main_program\r\n");
    if(isSuccessGetThreshold)
    {
      // 已经获取到阈值
      // 判断是否超过阈值
      alarmFlag = Utils_IsOverThreshold(&humi,&temp,&concentration);
      Serial_Printf("alarmFlag:\r\n%d\r\n", alarmFlag); 
      if(alarmFlag == 1 && alarmFlag != lastAlarmFlag)
      {
        // 温度超过阈值
        LED_On();
        OLED_Clear();
        OLED_ShowChinese(0,0,2);		// 温
        OLED_ShowChinese(0,16,4);		// 度
        OLED_ShowChinese(0,32,5);		// 过
        OLED_ShowChinese(0,48,6);		// 高

        // 发送短信
        ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
        ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
      }
      else if(alarmFlag == 2 && alarmFlag != lastAlarmFlag)
      {
        // 湿度超过阈值
        LED_On();
        OLED_Clear();
        OLED_ShowChinese(0,0,3);		// 湿
        OLED_ShowChinese(0,16,4);		// 度
        OLED_ShowChinese(0,32,5);		// 过
        OLED_ShowChinese(0,48,6);		// 高

        // 发送短信
        ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
        ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
      }
      else if(alarmFlag == 3 && alarmFlag != lastAlarmFlag)
      {
        // 浓度超过阈值
        LED_On();
        OLED_Clear();
        OLED_ShowChinese(0,0,27);		// 浓
        OLED_ShowChinese(0,16,4);		// 度
        OLED_ShowChinese(0,32,5);		// 过
        OLED_ShowChinese(0,48,6);   // 高

        // 发送短信
        ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
        ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
      }
      else if(alarmFlag == 0)
      {
        // 正常
        LED_Off();
        if(alarmFlag != lastAlarmFlag)
        {
          // 清屏
          OLED_Clear();
          // 显示汉字
          OLED_ShowChinese(0,0,2);		// 温
          OLED_ShowChinese(0,16,4);		// 度
          OLED_ShowString(1, 5, ":");
          OLED_ShowString(1, 9, ".");
          OLED_ShowChinese(2,0,3);		// 湿
          OLED_ShowChinese(2,16,4);		// 度
          OLED_ShowString(2, 5, ":");
          OLED_ShowString(2, 9, ".");
          OLED_ShowString(3,1,"PM2.5");
          OLED_ShowChinese(4,40,27);		// 浓
          OLED_ShowChinese(4,56,4);		// 度
          OLED_ShowString(3, 10, ":");
          OLED_ShowString(3, 14, ".");
        }
        // 显示温度、湿度、浓度数据
        OLED_ShowNum(1,7,temp.Temp_H,2);	 	                  // 温度 整数部分
        OLED_ShowNum(1,10,temp.Temp_L,1);		                  // 温度 小数部分
        OLED_ShowNum(2,7,humi.Humi_H,2);  	                  // 湿度 整数部分
        OLED_ShowNum(2,10,humi.Humi_L,1); 	                  // 湿度 小数部分
        OLED_ShowNum(3,11,concentration.Concentration_H,3);    // 浓度 整数部分
        OLED_ShowNum(3,15,concentration.Concentration_L,1);   // 浓度 小数部分
      }
      lastAlarmFlag = alarmFlag;	// 更新警报标志
    }
    else
    {
      // 没有获取到阈值
      // 显示温度、湿度、浓度数据
      OLED_ShowNum(1,7,temp.Temp_H,2);	 	                  // 温度 整数部分
      OLED_ShowNum(1,10,temp.Temp_L,1);		                  // 温度 小数部分
      OLED_ShowNum(2,7,humi.Humi_H,2);  	                  // 湿度 整数部分
      OLED_ShowNum(2,10,humi.Humi_L,1); 	                  // 湿度 小数部分
      OLED_ShowNum(3,11,concentration.Concentration_H,3);    // 浓度 整数部分
      OLED_ShowNum(3,15,concentration.Concentration_L,1);   // 浓度 小数部分
    }
    #endif
    // ---- 本地判断(主程序) ----

		// ---- ESP8266上传温湿度和pm2.5浓度数据 ----
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
      char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // 上传环境数据
      char* res_end;
      do
      {
        res = ESP8266_GetBuffer();
        res_end = strstr(res, "}");
      }while(res_end == NULL);
      char* res_json = strstr(res, "{");
      Serial_Printf("上传环境数据响应:\r\n%s\r\n", res_json); // 打印 JSON 数据
    }
    #endif
    // ---- ESP8266上传温湿度和pm2.5浓度数据 ----

    // ---- 红外 ----
    #if use_infrared
    Num_flag = Sensor_Get();
		OLED_ShowSignedNum(1, 7, Num_flag, 1);	//显示Num
    #endif
    // ---- 红外 ----

		// ---- 延时 ----
		Delay_ms(1000);
		// ---- 延时 ----
	}
}

void Mytiming_callback_1s(void)
{
  isUpdateData = 1;
  #if 0
	// ---- ESP8266获取阈值并更新 ----
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
    Serial_Printf("获取阈值:\r\n%s\r\n", res_json); // 打印 JSON 数据

    // 获取温度, 湿度, PM2.5浓度值
    char* name = "temperature";
    Utils_GetInformationFromResJson_8(res_json, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
    name = "humidity";
    Utils_GetInformationFromResJson_8(res_json, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
    name = "concentration";
    Utils_GetInformationFromResJson_16(res_json, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);
    isSuccessGetThreshold = 1;
  }
	// ---- ESP8266获取阈值并更新 ----

  // ---- ESP8266上传温湿度和pm2.5浓度数据 ----
  ESP8266_HTTP_PostDataTypestructure.temperature_H = temp.Temp_H;
  ESP8266_HTTP_PostDataTypestructure.temperature_L = temp.Temp_L;
  ESP8266_HTTP_PostDataTypestructure.humidity_H = humi.Humi_H;
  ESP8266_HTTP_PostDataTypestructure.humidity_L = humi.Humi_L;
  ESP8266_HTTP_PostDataTypestructure.concentration_H = pm;
  ESP8266_HTTP_PostDataTypestructure.concentration_L = 0;
  ESP8266_HTTP_PostDataTypestructure.timeout = 20;
  if(1)
  {
    char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // 上传环境数据
    char* res_end;
    do
    {
      res = ESP8266_GetBuffer();
      res_end = strstr(res, "}");
    }while(res_end == NULL);
    char* res_json = strstr(res, "{");
    Serial_Printf("上传环境数据响应:\r\n%s\r\n", res_json); // 打印 JSON 数据
  }
  // ---- ESP8266上传温湿度和pm2.5浓度数据 ----
  #endif
}

void Mytiming_callback_5s(void)
{
  isGetThreshold = 1;
  #if 0
	// ---- ESP8266获取阈值并更新 ----
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
    Serial_Printf("获取阈值:\r\n%s\r\n", res_json); // 打印 JSON 数据

    // 获取温度, 湿度, PM2.5浓度值
    char* name = "temperature";
    Utils_GetInformationFromResJson_8(res_json, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
    name = "humidity";
    Utils_GetInformationFromResJson_8(res_json, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
    name = "concentration";
    Utils_GetInformationFromResJson_16(res_json, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);
    isSuccessGetThreshold = 1;
  }
	// ---- ESP8266获取阈值并更新 ----

  // ---- ESP8266上传温湿度和pm2.5浓度数据 ----
  ESP8266_HTTP_PostDataTypestructure.temperature_H = temp.Temp_H;
  ESP8266_HTTP_PostDataTypestructure.temperature_L = temp.Temp_L;
  ESP8266_HTTP_PostDataTypestructure.humidity_H = humi.Humi_H;
  ESP8266_HTTP_PostDataTypestructure.humidity_L = humi.Humi_L;
  ESP8266_HTTP_PostDataTypestructure.concentration_H = pm;
  ESP8266_HTTP_PostDataTypestructure.concentration_L = 0;
  ESP8266_HTTP_PostDataTypestructure.timeout = 20;
  if(1)
  {
    char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // 上传环境数据
    char* res_end;
    do
    {
      res = ESP8266_GetBuffer();
      res_end = strstr(res, "}");
    }while(res_end == NULL);
    char* res_json = strstr(res, "{");
    Serial_Printf("上传环境数据响应:\r\n%s\r\n", res_json); // 打印 JSON 数据
  }
  // ---- ESP8266上传温湿度和pm2.5浓度数据 ----
  #endif
}

/**
 * @brief  TIM2中断函数处理函数, 1ms触发一次
 * @param  无
 * @retval 无
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
