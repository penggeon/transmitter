#include "utils.h"
#include "OLED.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Serial.h"
#define debug_mode_threshold 0

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
 * @brief  ��res����ȡ��Ϣ-threshold, ��������Ϊuint8_t
 * @param  res: ��Ӧ�ַ���
 * @param  name: ��������
              temperature: �¶�
              humidity: ʪ��
              concentration: Ũ��
 * @param  integer_part: ��������
 * @param  decimal_part: С������
 * @retval ��
 */
void Utils_GetInformationOfThreshold_8(char *res, char *name, uint8_t *integer_part, uint8_t *decimal_part) {
  #if debug_mode_threshold
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // Ѱ��name��ͷ
  #if debug_mode_threshold
  if(key_start==NULL)
  {
    Serial_Printf("error: û���ҵ�����name\r\n");
    return;
  }
  #endif
	char *result_start = strstr(key_start, "value") + 9;  // Ѱ�����ݿ�ͷ
	char *dot = strchr(result_start, '.');                // Ѱ��С����
  #if debug_mode_threshold
  if(dot==NULL)
  {
    Serial_Printf("error: û���ҵ�С����\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\"');    // Ѱ�����ݽ�β
  #if debug_mode_threshold
  if(result_end==NULL)
  {
    Serial_Printf("error: û���ҵ���β��ʶ��\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // ���������ַ���
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // С�������ַ���

	*integer_part = atoi(string_integer_part);      // ��������
	*decimal_part = atoi(string_decimal_part);      // С������

  #if debug_mode_threshold
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

/**
 * @brief  ��res����ȡ��Ϣ-threshold, ��������Ϊuint16_t
 * @param  res: ��Ӧ�ַ���
 * @param  name: ��������
              temperature: �¶�
              humidity: ʪ��
              concentration: Ũ��
 * @param  integer_part: ��������
 * @param  decimal_part: С������
 * @retval ��
 */
void Utils_GetInformationOfThreshold_16(char *res, char *name, uint16_t *integer_part, uint16_t *decimal_part) {
	#if debug_mode_threshold
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // Ѱ��name��ͷ
  #if debug_mode_threshold
  if(key_start==NULL)
  {
    Serial_Printf("error: û���ҵ�����name\r\n");
    return;
  }
  #endif
	char *result_start = strstr(key_start, "value") + 9;  // Ѱ�����ݿ�ͷ
	char *dot = strchr(result_start, '.');                // Ѱ��С����
  #if debug_mode_threshold
  if(dot==NULL)
  {
    Serial_Printf("error: û���ҵ�С����\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\"');    // Ѱ�����ݽ�β
  #if debug_mode_threshold
  if(result_end==NULL)
  {
    Serial_Printf("error: û���ҵ���β��ʶ��\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // ���������ַ���
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // С�������ַ���

	*integer_part = atoi(string_integer_part);      // ��������
	*decimal_part = atoi(string_decimal_part);      // С������

  #if debug_mode_threshold
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

/**
 * @brief  ��res����ȡ��Ϣ-thcp, ��������Ϊuint8_t
 * @param  res: thcp�е���Ӧ�ַ���
 * @param  name: ����
              temperature: �¶�
              humidity: ʪ��
              concentration: Ũ��
 * @param  integer_part: ��������
 * @param  decimal_part: С������
*/
void Utils_GetInformationOfThcp_8(char *res, char *name, uint8_t *integer_part, uint8_t *decimal_part)
{
  #if debug_mode_thcp
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // Ѱ��name��ͷ
  #if debug_mode_thcp
  if(key_start==NULL)
  {
    Serial_Printf("error: û���ҵ�����name\r\n");
    return;
  }
  #endif
	char *result_start = strchr(key_start, ':') + 4;  // Ѱ�����ݿ�ͷ
	char *dot = strchr(result_start, '.');            // Ѱ��С����
  #if debug_mode_thcp
  if(dot==NULL)
  {
    Serial_Printf("error: û���ҵ�С����\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\\');    // Ѱ�����ݽ�β
  #if debug_mode_thcp
  if(result_end==NULL)
  {
    Serial_Printf("error: û���ҵ���β��ʶ��\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // ���������ַ���
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // С�������ַ���

	*integer_part = atoi(string_integer_part);      // ��������
	*decimal_part = atoi(string_decimal_part);      // С������

  #if debug_mode_thcp
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

/**
 * @brief  ��res����ȡ��Ϣ-thcp, ��������Ϊuint16_t
 * @param  res: thcp�е���Ӧ�ַ���
 * @param  name: ����
              temperature: �¶�
              humidity: ʪ��
              concentration: Ũ��
 * @param  integer_part: ��������
 * @param  decimal_part: С������
*/
void Utils_GetInformationOfThcp_16(char *res, char *name, uint16_t *integer_part, uint16_t *decimal_part)
{
	#if debug_mode_thcp
  Serial_Printf("res:%s\r\n", res);
  #endif
	char *key_start = strstr(res, name);              // Ѱ��name��ͷ
  #if debug_mode_thcp
  if(key_start==NULL)
  {
    Serial_Printf("error: û���ҵ�����name\r\n");
    return;
  }
  #endif
	char *result_start = strchr(key_start, ':') + 4;  // Ѱ�����ݿ�ͷ
	char *dot = strchr(result_start, '.');            // Ѱ��С����
  #if debug_mode_thcp
  if(dot==NULL)
  {
    Serial_Printf("error: û���ҵ�С����\r\n");
    return;
  }
  #endif
	char *result_end = strchr(result_start, '\\');    // Ѱ�����ݽ�β
  #if debug_mode_thcp
  if(result_end==NULL)
  {
    Serial_Printf("error: û���ҵ���β��ʶ��\\\r\n");
    return;
  }
  #endif

	char string_integer_part[5];
	char string_decimal_part[5];
	strncpy(string_integer_part, result_start, dot - result_start); // ���������ַ���
	strncpy(string_decimal_part, dot + 1, result_end - dot - 1);    // С�������ַ���

	*integer_part = atoi(string_integer_part);      // ��������
	*decimal_part = atoi(string_decimal_part);      // С������

  #if debug_mode_thcp
  Serial_Printf("%s: %d.%d\r\n", name, *integer_part, *decimal_part);
  #endif
}

// ����PID������������
float Utils_PID_Error = 0;        // ��ǰ���
float Utils_PID_LastError = 0;    // ��һʱ�����
float Utils_PID_Integral = 0;     // ����ֵ
float Utils_PID_Differential = 0; // ΢��ֵ
float Utils_PID_Output = 0;       // ���ֵ
// ����PID�����ϵ����ֵ
float Utils_PID_Kp = 20;          // ����ϵ��
float Utils_PID_Ki = 0;           // ����ϵ��
float Utils_PID_Kd = 0;           // ΢��ϵ��
float Utils_PID_DeadZone = 5;     // �������ֵ

/**
 * @brief  PID����õ��ٶ����ֵ
 * @param  temperature_H: �¶ȸ�λ
 * @param  temperature_L: �¶ȵ�λ
 * @param  temperature_H_Threshold: �¶���ֵ��λ
 * @param  temperature_L_Threshold: �¶���ֵ��λ
 * @retval �ٶ����ֵ
*/
int8_t Utils_GetPIDOutputSpeed(uint8_t temperature_H, uint8_t temperature_L, uint8_t temperature_H_Threshold, uint8_t temperature_L_Threshold)
{
  // ������������С�����ֺϳ�һ��������
  char temp[50];
  float temperature, temperature_Threshold;
  sprintf(temp, "%d.%d", temperature_H, temperature_L);
  sscanf(temp, "%f", &temperature);
  sprintf(temp, "%d.%d", temperature_H_Threshold, temperature_L_Threshold);
  sscanf(temp, "%f", &temperature_Threshold);

  // ��������ֵ
  Utils_PID_Error = temperature - temperature_Threshold;            // ������
  Utils_PID_Integral += Utils_PID_Error;                            // �ۻ����
  Utils_PID_Differential = Utils_PID_Error - Utils_PID_LastError;   // ΢�����

  // PID�������, ����õ��ٶ�ֵ
  Utils_PID_Output = Utils_PID_Error * Utils_PID_Kp +
                    Utils_PID_Integral * Utils_PID_Ki + 
                    Utils_PID_Differential * Utils_PID_Kd;

  // �������
  Utils_PID_LastError = Utils_PID_Error;

  // ����޷�
  if(Utils_PID_Output > 99)
  {
    Utils_PID_Output = 99;
  }
  else if(Utils_PID_Output < 0)
  {
    Utils_PID_Output = 0;
  }

  // �������
  if(Utils_PID_Output < Utils_PID_DeadZone)
  {
    Utils_PID_Output = 0;
  }

  // �����ٶ�ֵ
  return (uint8_t)Utils_PID_Output;
}
