#include "utils.h"
#include "OLED.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
 * @brief  从res_json中获取温度和湿度信息
 * @param  res_json: 响应JSON字符串
 * @param  name: 数据名称
              temperature: 温度
              humidity: 湿度
 * @param  integer_part: 整数部分
 * @param  decimal_part: 小数部分
 * @retval 无
 */
void Utils_GetInformationFromResJson_8(char* res_json, char* name, uint8_t* integer_part, uint8_t* decimal_part)
{
  char temp[100];
  sprintf(temp, "\"name\": \"%s\"", name);
  char *name_start = strstr(res_json, temp);
  if (name_start != NULL)
  {
    // 找到"name": "temperature"的位置
    char *value_start = strstr(name_start, "\"value\": ");
    if (value_start != NULL)
    {
      // 找到"value": "的位置"
      value_start += strlen("\"value\": ");
      // 提取name的值
      char *end = strchr(value_start, '}'); // 找到name的值结束的位置
      if (end != NULL)
      {
        // 复制name的值到一个新的字符串中
        char result[20];
        strncpy(result, value_start, end - value_start);
        result[end - value_start] = '\0';

        char *decimal_separator = strchr(result, '.'); // 查找小数点位置
        if (decimal_separator != NULL)
        {
          *decimal_separator = '\0'; // 将小数点替换为字符串结束符

          // 将整数部分和小数部分转换为整数
          *integer_part = atoi(result);
          *decimal_part = atoi(decimal_separator + 1);
        }
        // else
        // {
        //   Serial_Printf("无法找到小数点\n");
        // }

        // 打印
        // Serial_Printf("温度值为：%s\n", temperature);
      }
    }
  }
}

/**
 * @brief  从res_json中获取温度和湿度信息
 * @param  res_json: 响应JSON字符串
 * @param  name: 数据名称
              temperature: 温度
              humidity: 湿度
 * @param  integer_part: 整数部分
 * @param  decimal_part: 小数部分
 * @retval 无
 */
void Utils_GetInformationFromResJson_16(char* res_json, char* name, uint16_t* integer_part, uint16_t* decimal_part)
{
  char temp[100];
  sprintf(temp, "\"name\": \"%s\"", name);
  char *name_start = strstr(res_json, temp);
  if (name_start != NULL)
  {
    // 找到"name": "temperature"的位置
    char *value_start = strstr(name_start, "\"value\": ");
    if (value_start != NULL)
    {
      // 找到"value": "的位置"
      value_start += strlen("\"value\": ");
      // 提取name的值
      char *end = strchr(value_start, '}'); // 找到name的值结束的位置
      if (end != NULL)
      {
        // 复制name的值到一个新的字符串中
        char result[20];
        strncpy(result, value_start, end - value_start);
        result[end - value_start] = '\0';

        char *decimal_separator = strchr(result, '.'); // 查找小数点位置
        if (decimal_separator != NULL)
        {
          *decimal_separator = '\0'; // 将小数点替换为字符串结束符

          // 将整数部分和小数部分转换为整数
          *integer_part = atoi(result);
          *decimal_part = atoi(decimal_separator + 1);
        }
        // else
        // {
        //   Serial_Printf("无法找到小数点\n");
        // }

        // 打印
        // Serial_Printf("温度值为：%s\n", temperature);
      }
    }
  }
}
