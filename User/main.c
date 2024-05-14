#include "stm32f10x.h"                  // Device header
#include <string.h>
#include <stdlib.h>
#include "Delay.h"                      // 延时函数
#include "Serial.h"                     // 串口
#include "utils.h"                      // 工具函数
#include "mytiming.h"                   // 定时器
#include "OLED.h"                       // 模块 - OLED
#include "DHT11.h"                      // 模块 - DHT11
#include "LED.h"                        // 模块 - LED
#include "ESP8266.h"                    // 模块 - ESP8266
#include "gp2y.h"                       // 模块 - GP2Y1014AU
#include "Sensor.h"                     // 模块 - 红外传感器
#include "Motor.h"                      // 模块 - 电机

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
uint16_t pm;        // PM2.5浓度
int8_t Speed;       // 点击转速(-100 ~ +100)

// 标识
uint8_t mainProgramSuccessInit = 0;       // 整体程序是否完成初始化
uint8_t ESP8266GetThresholdSuccess = 0;   // ESP8266是否获取到阈值(状态)
uint8_t ESP8266GetThreshold = 0;          // ESP8266是否获取阈值(动作)
uint8_t ESP8266UpdateData = 0;            // ESP8266是否上传数据(动作)
// ---- 定义全局变量 ----

int main(void)
{
	// 初始化前配置NVIC中断分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			// 配置NVIC为分组2

	// 外设初始化
	Serial_Init();		    // 串口初始化
  Mytiming_Init();      // 定时器初始化
	OLED_Init();			    // 模块 - OLED初始化
	DH11_GPIO_Init();     // 模块 - DHT11初始化
	LED_Init();				    // 模块 - LED初始化
	ESP8266_Init();	      // 模块 - ESP8266初始化
	GP2Y1014AU_init();    // 模块 - GP2Y1014AU初始化
  Sensor_Init();		    // 模块 - 红外传感器初始化
  Motor_Init();		      // 模块 - 电机初始化

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
  ESP8266_InitStructure.ESP8266_SERVER_ADDRESS = "8.130.11.139";
                                        // "116.62.81.138": 心知天气服务器IP地址
                                        // "192.168.41.242": 局域网内的电脑TCP客户端IP地址
                                        // "47.93.208.134": 测试综合实验服务器IP地址
                                        // "8.130.11.139": 最终综合实验服务器IP地址
  ESP8266_InitStructure.ESP8266_SERVER_PORT = 8080;
  uint8_t res_ESP8266Connetion = ESP8266_InitConnection(&ESP8266_InitStructure); // 连接服务器
  if(!res_ESP8266Connetion)
  {
  	OLED_Clear();   // 连接成功, 清空OLED
  }
  #endif
  // ---- ESP8266软件初始化 ----

  // ---- DHT11 ----
  #if use_dht11
	// 显示温湿度三个字
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

  mainProgramSuccessInit = 1; // 完成初始化

	while (1)
	{
    // ---- ESP8266获取阈值并更新 ----
    #if use_esp8266
    if(ESP8266GetThreshold)
    {
      Serial_Printf("ESP8266 获取阈值.\r\n");
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

      // 获取温度, 湿度, PM2.5浓度值
      char* name = "temperature";
      Utils_GetInformationOfThreshold_8(res, name, &temp.Temp_H_Threshold, &temp.Temp_L_Threshold);
      name = "humidity";
      Utils_GetInformationOfThreshold_8(res, name, &humi.Humi_H_Threshold, &humi.Humi_L_Threshold);
      name = "concentration";
      Utils_GetInformationOfThreshold_16(res, name, &concentration.Concentration_H_Threshold, &concentration.Concentration_L_Threshold);
      // Serial_Printf("设置完本地数据\r\n");
      // Serial_Printf("temp*:%d.%d\r\n", temp.Temp_H_Threshold, temp.Temp_L_Threshold);
      // Serial_Printf("humi*:%d.%d\r\n", humi.Humi_H_Threshold, humi.Humi_L_Threshold);
      // Serial_Printf("concentration*:%d.%d\r\n", concentration.Concentration_H_Threshold, concentration.Concentration_L_Threshold);

      // 更新标志位
      ESP8266GetThresholdSuccess = 1;
      ESP8266GetThreshold = 0;
    }
    #endif
    // ---- ESP8266获取阈值并更新 ----

    // ---- gp2y获取 ----
    #if use_gp2y
    // Serial_Printf("gp2y\r\n");
    pm = GP2Y1014AU_GetInfo();   // 获取浓度信息
    concentration.Concentration_H = pm;  // 整数部分
    concentration.Concentration_L = 0;   // 小数部分
    #endif
    // ---- gp2y获取 ----

		// ---- DHT11 ----
    #if use_dht11
    // Serial_Printf("dht11\r\n");
		DHT11ReadData(&temp.Temp_H,&temp.Temp_L,&humi.Humi_H,&humi.Humi_L); // 获取数据
    #endif
		// ---- DHT11 ----

    // ---- 本地判断(主程序) ----
    #if main_program
    // Serial_Printf("main_program\r\n");
    if(ESP8266GetThresholdSuccess)
    {
      // 已经获取到阈值
      // 判断是否超过阈值
      alarmFlag = Utils_IsOverThreshold(&humi,&temp,&concentration);
      Serial_Printf("温度数据: %d, %d, %d, %d\r\n", temp.Temp_H, temp.Temp_L, temp.Temp_H_Threshold, temp.Temp_L_Threshold);
      Serial_Printf("alarmFlag: %d\r\n", alarmFlag);
      if(alarmFlag == 1)
      {
        // 当前温度超过阈值
        if(alarmFlag != lastAlarmFlag)
        {
          // 初次判断为温度过高, 即以下内容只需执行一次
          // LED_On();
          OLED_Clear();
          // 温度过高
          OLED_ShowChinese(0,0,2);		// 温
          OLED_ShowChinese(0,16,4);		// 度
          OLED_ShowChinese(0,32,5);		// 过
          OLED_ShowChinese(0,48,6);		// 高

          // 发送短信
          // ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
          // ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
          Serial_Printf("温度过高\r\n");
        }
        // 显示具体温度值
        OLED_ShowChinese(2,0,2);		// 温
        OLED_ShowChinese(2,16,4);		// 度
        OLED_ShowString(2, 5, ":");
        OLED_ShowString(2, 9, ".");
        OLED_ShowNum(2,7,temp.Temp_H,2);    // 温度 整数部分
        OLED_ShowNum(2,10,temp.Temp_L,1);   // 温度 小数部分

        // 风扇转动
        Speed = Utils_GetPIDOutputSpeed(temp.Temp_H, temp.Temp_L, temp.Temp_H_Threshold, temp.Temp_L_Threshold);
        Motor_SetSpeed(Speed);
      }
      else if(alarmFlag == 2)
      {
        // 当前湿度超过阈值
        if(alarmFlag != lastAlarmFlag)
        {
          // LED_On();
          OLED_Clear();
          // 湿度过高
          OLED_ShowChinese(0,0,3);		// 湿
          OLED_ShowChinese(0,16,4);		// 度
          OLED_ShowChinese(0,32,5);		// 过
          OLED_ShowChinese(0,48,6);		// 高

          // 发送短信
          // ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
          // ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
          Serial_Printf("湿度过高\r\n");
        }
        // 显示具体湿度值
        OLED_ShowChinese(2,0,3);		// 湿
        OLED_ShowChinese(2,16,4);		// 度
        OLED_ShowString(2, 5, ":");
        OLED_ShowString(2, 9, ".");
        OLED_ShowNum(2,7,humi.Humi_H,2);    // 湿度 整数部分
        OLED_ShowNum(2,10,humi.Humi_L,1);   // 湿度 小数部分
      }
      else if(alarmFlag == 3)
      {
        // 当前浓度超过阈值
        if(alarmFlag != lastAlarmFlag)
        {
          // LED_On();
          OLED_Clear();
          // 浓度过高
          OLED_ShowChinese(0,0,27);		// 浓
          OLED_ShowChinese(0,16,4);		// 度
          OLED_ShowChinese(0,32,5);		// 过
          OLED_ShowChinese(0,48,6);   // 高

          // 发送短信
          // ESP8266_HTTP_SendSmsTypestructure.timeout = 20;
          // ESP8266_HTTPSendSms(&ESP8266_HTTP_SendSmsTypestructure);
          Serial_Printf("浓度过高\r\n");
        }
        // 显示具体浓度值
        OLED_ShowString(2,1,"PM2.5");
        OLED_ShowChinese(2,40,27);	// 浓
        OLED_ShowChinese(2,56,4);		// 度
        OLED_ShowString(2, 10, ":");
        OLED_ShowString(2, 14, ".");
        OLED_ShowNum(2,11,concentration.Concentration_H,3);   // 浓度 整数部分
        OLED_ShowNum(2,15,concentration.Concentration_L,1);   // 浓度 小数部分
      }
      else if(alarmFlag == 0)
      {
        // 当前正常
        LED_Off();
        if(alarmFlag != lastAlarmFlag)
        {
          // 清屏
          OLED_Clear();
          // 显示汉字
          OLED_ShowChinese(0,0,2);		  // 温
          OLED_ShowChinese(0,16,4);		  // 度
          OLED_ShowString(1, 5, ":");
          OLED_ShowString(1, 9, ".");
          OLED_ShowChinese(2,0,3);		  // 湿
          OLED_ShowChinese(2,16,4);		  // 度
          OLED_ShowString(2, 5, ":");
          OLED_ShowString(2, 9, ".");
          OLED_ShowString(3,1,"PM2.5");
          OLED_ShowChinese(4,40,27);		// 浓
          OLED_ShowChinese(4,56,4);		  // 度
          OLED_ShowString(3, 10, ":");
          OLED_ShowString(3, 14, ".");
          Serial_Printf("温湿浓度正常.\r\n");
        }
        // 显示温度、湿度、浓度数据
        OLED_ShowNum(1,7,temp.Temp_H,2);	 	                  // 温度 整数部分
        OLED_ShowNum(1,10,temp.Temp_L,1);		                  // 温度 小数部分
        OLED_ShowNum(2,7,humi.Humi_H,2);  	                  // 湿度 整数部分
        OLED_ShowNum(2,10,humi.Humi_L,1); 	                  // 湿度 小数部分
        OLED_ShowNum(3,11,concentration.Concentration_H,3);   // 浓度 整数部分
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
      OLED_ShowNum(3,11,concentration.Concentration_H,3);   // 浓度 整数部分
      OLED_ShowNum(3,15,concentration.Concentration_L,1);   // 浓度 小数部分
    }
    #endif
    // ---- 本地判断(主程序) ----

		// ---- ESP8266上传温室浓度数据 ----
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
      Serial_Printf("ESP8266 上传温湿浓度数据.\r\n");
      char* res = ESP8266_HTTPPostData(&ESP8266_HTTP_PostDataTypestructure); // 上传环境数据
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

      // 更新标志位
      ESP8266UpdateData = 0;
    }
    #endif
    // ---- ESP8266上传温室浓度数据 ----

    // ---- 红外 ----
    #if use_infrared
    Num_flag = Sensor_Get();
		OLED_ShowSignedNum(1, 7, Num_flag, 1);	//显示Num
    #endif
    // ---- 红外 ----

		// ---- 延时 ----
		// Delay_ms(1000);
		// ---- 延时 ----
	}
}

// ---- 计时 ----

void Mytiming_callback_1s(void)
{

}

void Mytiming_callback_3s(void)
{
  ESP8266UpdateData = 1;    // 上传温室浓度数据
}

void Mytiming_callback_5s(void)
{
  ESP8266GetThreshold = 1;  // 获取阈值
}

// 计时用计数器
uint32_t Mytiming_count_1s = 0;
uint32_t Mytiming_count_3s = 0;
uint32_t Mytiming_count_5s = 0;

/**
 * @brief  TIM2中断函数处理函数, 1ms触发一次
 * @param  无
 * @retval 无
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

      // 计数器增加
      Mytiming_count_1s++;
      Mytiming_count_3s++;
      Mytiming_count_5s++;
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}
// ---- 计时 ----
