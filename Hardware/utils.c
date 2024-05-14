#include "utils.h"
#include "OLED.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Serial.h"
#define debug_mode_threshold 0

/**
 * @brief  判断湿度是否超过阈值
 * @param  humi:湿度结构体地址
 * @retval 1:超过阈值; 0:未超过阈值
 */
uint8_t IsHumidityOverThreshold(Humidity_Typedef* humi)
{
  if(humi->Humi_H > humi->Humi_H_Threshold)
  {
    return 1;
  }
  if(humi->Humi_H == humi->Humi_H_Threshold && humi->Humi_L >= humi->Humi_L_Threshold)
  {
    return 1;
  } 
  return 0;
}

/**
 * @brief  判断温度是否超过阈值
 * @param  temp:温度结构体地址
 * @retval 1:超过阈值; 0:未超过阈值
 */
uint8_t IsTemperatureOverThreshold(Temperature_Typedef* temp)
{
  if(temp->Temp_H > temp->Temp_H_Threshold)
  {
    return 1;
  }
  if(temp->Temp_H == temp->Temp_H_Threshold && temp->Temp_L>= temp->Temp_L_Threshold)
  {
    return 1;
  }
  return 0;
}

/**
 * @brief  判断浓度是否超过阈值
 * @param  concentration:浓度结构体地址
 * @retval 1:超过阈值; 0:未超过阈值
 */
uint8_t IsConcentrationOverThreshold(Concentration_Typedef* concentration)
{
  if(concentration->Concentration_H > concentration->Concentration_H_Threshold)
  {
    return 1;
  }
  if(concentration->Concentration_H == concentration->Concentration_H_Threshold && concentration->Concentration_L >= concentration->Concentration_L_Threshold)
  {
    return 1;
  } 
  return 0;
}

/**
 * @brief  判断是否超过阈值
 * @param  humi:湿度结构体地址
 * @param  temp:温度结构体地址
 * @param  concentration:浓度结构体地址
 * @retval 1:温度超过阈值; 2:湿度超过阈值; 3:浓度超过阈值; 0:未超过阈值
 */
uint8_t Utils_IsOverThreshold(Humidity_Typedef* Humi, Temperature_Typedef* Temp, Concentration_Typedef* Concentration)
{
  if(IsTemperatureOverThreshold(Temp))
  {
    return 1;
  }
  if(IsHumidityOverThreshold(Humi))
  {
    return 2;
  }
  if(IsConcentrationOverThreshold(Concentration))
  {
    return 3;
  }
  return 0;
}

/**
 * @brief  显示欢迎信息
 * @param  无
 * @retval 无
 */
void Utils_ShowWelcomeMessage(void)
{
  OLED_ShowChinese(2,16,7);		// 通
	OLED_ShowChinese(2,32,8);		// 信
	OLED_ShowChinese(2,48,17);	// 机
	OLED_ShowChinese(2,64,18);	// 房
	OLED_ShowChinese(2,80,19);	// 环
	OLED_ShowChinese(2,96,20);	// 境
	OLED_ShowChinese(4,16,21);	// 自
	OLED_ShowChinese(4,32,22);	// 动
	OLED_ShowChinese(4,48,23);	// 监
	OLED_ShowChinese(4,64,24);	// 测
	OLED_ShowChinese(4,80,25);	// 系
	OLED_ShowChinese(4,96,26);	// 统
}

/**
 * @brief  从res中提取信息-threshold, 数据类型为uint8_t
 * @param  res: 响应字符串
 * @param  name: 数据名称
              temperature: 温度
              humidity: 湿度
              concentration: 浓度
 * @param  integer_part: 整数部分
 * @param  decimal_part: 小数部分
 * @retval 无
 */
void Utils_GetInformationOfThreshold_8(char *res, char *name, uint8_t *integer_part, uint8_t *decimal_part) {
  #if debug_mode_threshold
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // 寻找name开头
  #if debug_mode_threshold
  if(key_start==NULL)
  {
    Serial_Printf("error: 没有找到参数name\r\n");
    return;
  }
  #endif
	char *result_start = strstr(key_start, "value") + 9;  // 寻找数据开头
	char *dot = strchr(result_start, '.');                // 寻找小数点
  #if debug_mode_threshold
  if(dot==NULL)
  {
    Serial_Printf("error: 没有找到小数点\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\"');    // 寻找数据结尾
  #if debug_mode_threshold
  if(result_end==NULL)
  {
    Serial_Printf("error: 没有找到结尾标识符\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // 整数部分字符串
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // 小数部分字符串

	*integer_part = atoi(string_integer_part);      // 整数部分
	*decimal_part = atoi(string_decimal_part);      // 小数部分

  #if debug_mode_threshold
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

/**
 * @brief  从res中提取信息-threshold, 数据类型为uint16_t
 * @param  res: 响应字符串
 * @param  name: 数据名称
              temperature: 温度
              humidity: 湿度
              concentration: 浓度
 * @param  integer_part: 整数部分
 * @param  decimal_part: 小数部分
 * @retval 无
 */
void Utils_GetInformationOfThreshold_16(char *res, char *name, uint16_t *integer_part, uint16_t *decimal_part) {
	#if debug_mode_threshold
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // 寻找name开头
  #if debug_mode_threshold
  if(key_start==NULL)
  {
    Serial_Printf("error: 没有找到参数name\r\n");
    return;
  }
  #endif
	char *result_start = strstr(key_start, "value") + 9;  // 寻找数据开头
	char *dot = strchr(result_start, '.');                // 寻找小数点
  #if debug_mode_threshold
  if(dot==NULL)
  {
    Serial_Printf("error: 没有找到小数点\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\"');    // 寻找数据结尾
  #if debug_mode_threshold
  if(result_end==NULL)
  {
    Serial_Printf("error: 没有找到结尾标识符\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // 整数部分字符串
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // 小数部分字符串

	*integer_part = atoi(string_integer_part);      // 整数部分
	*decimal_part = atoi(string_decimal_part);      // 小数部分

  #if debug_mode_threshold
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

/**
 * @brief  从res中提取信息-thcp, 数据类型为uint8_t
 * @param  res: thcp中的响应字符串
 * @param  name: 名称
              temperature: 温度
              humidity: 湿度
              concentration: 浓度
 * @param  integer_part: 整数部分
 * @param  decimal_part: 小数部分
*/
void Utils_GetInformationOfThcp_8(char *res, char *name, uint8_t *integer_part, uint8_t *decimal_part)
{
  #if debug_mode_thcp
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // 寻找name开头
  #if debug_mode_thcp
  if(key_start==NULL)
  {
    Serial_Printf("error: 没有找到参数name\r\n");
    return;
  }
  #endif
	char *result_start = strchr(key_start, ':') + 4;  // 寻找数据开头
	char *dot = strchr(result_start, '.');            // 寻找小数点
  #if debug_mode_thcp
  if(dot==NULL)
  {
    Serial_Printf("error: 没有找到小数点\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\\');    // 寻找数据结尾
  #if debug_mode_thcp
  if(result_end==NULL)
  {
    Serial_Printf("error: 没有找到结尾标识符\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // 整数部分字符串
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // 小数部分字符串

	*integer_part = atoi(string_integer_part);      // 整数部分
	*decimal_part = atoi(string_decimal_part);      // 小数部分

  #if debug_mode_thcp
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

/**
 * @brief  从res中提取信息-thcp, 数据类型为uint16_t
 * @param  res: thcp中的响应字符串
 * @param  name: 名称
              temperature: 温度
              humidity: 湿度
              concentration: 浓度
 * @param  integer_part: 整数部分
 * @param  decimal_part: 小数部分
*/
void Utils_GetInformationOfThcp_16(char *res, char *name, uint16_t *integer_part, uint16_t *decimal_part)
{
	#if debug_mode_thcp
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // 寻找name开头
  #if debug_mode_thcp
  if(key_start==NULL)
  {
    Serial_Printf("error: 没有找到参数name\r\n");
    return;
  }
  #endif
	char *result_start = strchr(key_start, ':') + 4;  // 寻找数据开头
	char *dot = strchr(result_start, '.');            // 寻找小数点
  #if debug_mode_thcp
  if(dot==NULL)
  {
    Serial_Printf("error: 没有找到小数点\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\\');    // 寻找数据结尾
  #if debug_mode_thcp
  if(result_end==NULL)
  {
    Serial_Printf("error: 没有找到结尾标识符\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // 整数部分字符串
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // 小数部分字符串

	*integer_part = atoi(string_integer_part);      // 整数部分
	*decimal_part = atoi(string_decimal_part);      // 小数部分

  #if debug_mode_thcp
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

// 定义PID计算所用数据
float Utils_PID_Error = 0;        // 当前误差
float Utils_PID_LastError = 0;    // 上一时刻误差
float Utils_PID_Integral = 0;     // 积分值
float Utils_PID_Differential = 0; // 微分值
float Utils_PID_Output = 0;       // 输出值
// 定义PID计算的系数和值
float Utils_PID_Kp = 20;          // 比例系数
float Utils_PID_Ki = 0;           // 积分系数
float Utils_PID_Kd = 0;           // 微分系数
float Utils_PID_DeadZone = 5;     // 输出死区值

/**
 * @brief  PID计算得到速度输出值
 * @param  temperature_H: 温度高位
 * @param  temperature_L: 温度低位
 * @param  temperature_H_Threshold: 温度阈值高位
 * @param  temperature_L_Threshold: 温度阈值低位
 * @retval 速度输出值
*/
int8_t Utils_GetPIDOutputSpeed(uint8_t temperature_H, uint8_t temperature_L, uint8_t temperature_H_Threshold, uint8_t temperature_L_Threshold)
{
  // 将整数部分与小数部分合成一个浮点数
  char temp[50];
  float temperature, temperature_Threshold;
  sprintf(temp, "%d.%d", temperature_H, temperature_L);
  sscanf(temp, "%f", &temperature);
  sprintf(temp, "%d.%d", temperature_H_Threshold, temperature_L_Threshold);
  sscanf(temp, "%f", &temperature_Threshold);

  // 计算所需值
  Utils_PID_Error = temperature - temperature_Threshold;            // 误差计算
  Utils_PID_Integral += Utils_PID_Error;                            // 累积误差
  Utils_PID_Differential = Utils_PID_Error - Utils_PID_LastError;   // 微分误差

  // PID计算过程, 计算得到速度值
  Utils_PID_Output = Utils_PID_Error * Utils_PID_Kp +
                    Utils_PID_Integral * Utils_PID_Ki + 
                    Utils_PID_Differential * Utils_PID_Kd;

  // 更新误差
  Utils_PID_LastError = Utils_PID_Error;

  // 输出限幅
  if(Utils_PID_Output > 99)
  {
    Utils_PID_Output = 99;
  }
  else if(Utils_PID_Output < 0)
  {
    Utils_PID_Output = 0;
  }

  // 输出死区
  if(Utils_PID_Output < Utils_PID_DeadZone)
  {
    Utils_PID_Output = 0;
  }

  // 返回速度值
  return (uint8_t)Utils_PID_Output;
}
