#include "utils.h"
#include "OLED.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief  �ж�ʪ���Ƿ񳬹���ֵ
 * @param  humi:ʪ�Ƚṹ���ַ
 * @retval 1:������ֵ; 0:δ������ֵ
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
 * @brief  �ж��¶��Ƿ񳬹���ֵ
 * @param  temp:�¶Ƚṹ���ַ
 * @retval 1:������ֵ; 0:δ������ֵ
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
 * @brief  �ж�Ũ���Ƿ񳬹���ֵ
 * @param  concentration:Ũ�Ƚṹ���ַ
 * @retval 1:������ֵ; 0:δ������ֵ
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
 * @brief  �ж��Ƿ񳬹���ֵ
 * @param  humi:ʪ�Ƚṹ���ַ
 * @param  temp:�¶Ƚṹ���ַ
 * @param  concentration:Ũ�Ƚṹ���ַ
 * @retval 1:�¶ȳ�����ֵ; 2:ʪ�ȳ�����ֵ; 3:Ũ�ȳ�����ֵ; 0:δ������ֵ
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
 * @brief  ��ʾ��ӭ��Ϣ
 * @param  ��
 * @retval ��
 */
void Utils_ShowWelcomeMessage(void)
{
  OLED_ShowChinese(2,16,7);		// ͨ
	OLED_ShowChinese(2,32,8);		// ��
	OLED_ShowChinese(2,48,17);	// ��
	OLED_ShowChinese(2,64,18);	// ��
	OLED_ShowChinese(2,80,19);	// ��
	OLED_ShowChinese(2,96,20);	// ��
	OLED_ShowChinese(4,16,21);	// ��
	OLED_ShowChinese(4,32,22);	// ��
	OLED_ShowChinese(4,48,23);	// ��
	OLED_ShowChinese(4,64,24);	// ��
	OLED_ShowChinese(4,80,25);	// ϵ
	OLED_ShowChinese(4,96,26);	// ͳ
}

/**
 * @brief  ��res_json�л�ȡ�¶Ⱥ�ʪ����Ϣ
 * @param  res_json: ��ӦJSON�ַ���
 * @param  name: ��������
              temperature: �¶�
              humidity: ʪ��
 * @param  integer_part: ��������
 * @param  decimal_part: С������
 * @retval ��
 */
void Utils_GetInformationFromResJson_8(char* res_json, char* name, uint8_t* integer_part, uint8_t* decimal_part)
{
  char temp[100];
  sprintf(temp, "\"name\": \"%s\"", name);
  char *name_start = strstr(res_json, temp);
  if (name_start != NULL)
  {
    // �ҵ�"name": "temperature"��λ��
    char *value_start = strstr(name_start, "\"value\": ");
    if (value_start != NULL)
    {
      // �ҵ�"value": "��λ��"
      value_start += strlen("\"value\": ");
      // ��ȡname��ֵ
      char *end = strchr(value_start, '}'); // �ҵ�name��ֵ������λ��
      if (end != NULL)
      {
        // ����name��ֵ��һ���µ��ַ�����
        char result[20];
        strncpy(result, value_start, end - value_start);
        result[end - value_start] = '\0';

        char *decimal_separator = strchr(result, '.'); // ����С����λ��
        if (decimal_separator != NULL)
        {
          *decimal_separator = '\0'; // ��С�����滻Ϊ�ַ���������

          // ���������ֺ�С������ת��Ϊ����
          *integer_part = atoi(result);
          *decimal_part = atoi(decimal_separator + 1);
        }
        // else
        // {
        //   Serial_Printf("�޷��ҵ�С����\n");
        // }

        // ��ӡ
        // Serial_Printf("�¶�ֵΪ��%s\n", temperature);
      }
    }
  }
}

/**
 * @brief  ��res_json�л�ȡ�¶Ⱥ�ʪ����Ϣ
 * @param  res_json: ��ӦJSON�ַ���
 * @param  name: ��������
              temperature: �¶�
              humidity: ʪ��
 * @param  integer_part: ��������
 * @param  decimal_part: С������
 * @retval ��
 */
void Utils_GetInformationFromResJson_16(char* res_json, char* name, uint16_t* integer_part, uint16_t* decimal_part)
{
  char temp[100];
  sprintf(temp, "\"name\": \"%s\"", name);
  char *name_start = strstr(res_json, temp);
  if (name_start != NULL)
  {
    // �ҵ�"name": "temperature"��λ��
    char *value_start = strstr(name_start, "\"value\": ");
    if (value_start != NULL)
    {
      // �ҵ�"value": "��λ��"
      value_start += strlen("\"value\": ");
      // ��ȡname��ֵ
      char *end = strchr(value_start, '}'); // �ҵ�name��ֵ������λ��
      if (end != NULL)
      {
        // ����name��ֵ��һ���µ��ַ�����
        char result[20];
        strncpy(result, value_start, end - value_start);
        result[end - value_start] = '\0';

        char *decimal_separator = strchr(result, '.'); // ����С����λ��
        if (decimal_separator != NULL)
        {
          *decimal_separator = '\0'; // ��С�����滻Ϊ�ַ���������

          // ���������ֺ�С������ת��Ϊ����
          *integer_part = atoi(result);
          *decimal_part = atoi(decimal_separator + 1);
        }
        // else
        // {
        //   Serial_Printf("�޷��ҵ�С����\n");
        // }

        // ��ӡ
        // Serial_Printf("�¶�ֵΪ��%s\n", temperature);
      }
    }
  }
}
