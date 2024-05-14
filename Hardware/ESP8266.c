#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "delay.h"
#include "usart.h"
#include "ESP8266.h"
#include "Serial.h"
#include <string.h>
#include "OLED.h"
#define debug_mode 0	// 1: �򿪵���ģʽ, 0: �رյ���ģʽ
#define output_mode 1 // 1: �������������, 0: �ر������������

USART_Buffer ESP8266_Buffer; // ����һ��USART_Buffer�ṹ�����

/**
 * @brief  ESP8266ģ���ʼ��, �˺�����������Ӳ����·��ʼ��
 * @param  None
 * @retval None
 */
void ESP8266_Init(void)
{
  Usart3_Init(115200);      // ����3��ʼ��
  ESP8266_RST(SET);   // ��λ��ƽ����, ��ģ�鲻��λ
  ESP8266_CH_PD(SET); // ʹ��ģ��
}

/**
 * @brief  ���ڷ���һ���ֽ�
 * @param  Byte Ҫ���͵��ֽ�����
 * @retval None
 */
void ESP8266_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);		// ���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	// �ȴ��������
	/*�´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ*/
}

/**
 * @brief  ���ڷ���һ���ַ���
 * @param  string Ҫ�����ַ������׵�ַ
 * @retval None
 */
void ESP8266_SendString(char *string)
{
  uint32_t i;
	for (i = 0; string[i] != '\0'; i ++)  // �����ַ����飨�ַ������������ַ���������־λ��ֹͣ
	{
		ESP8266_SendByte(string[i]);		    // ���ε���Serial_SendByte����ÿ���ֽ�����
	}
}

/**
 * @brief  �Լ���װ��prinf����
 * @param  format ��ʽ���ַ���
 * @param  ... �ɱ�Ĳ����б�
 * @retval None
*/
void ESP8266_Printf(char *format, ...)
{
	char String[1024];      //�����ַ�����
	va_list arg;            //����ɱ�����б��������͵ı���arg
	va_start(arg, format);  //��format��ʼ�����ղ����б�arg����
	vsprintf(String, format, arg);    //ʹ��vsprintf��ӡ��ʽ���ַ����Ͳ����б��ַ�������
	va_end(arg);                      //��������arg
	ESP8266_SendString(String);       //���ڷ����ַ����飨�ַ�����
}

/**
 * @brief  ���ESP8266_Buffer�ṹ��
 * @param  None
 * @retval None
 */
void ESP8266_ClearBuffer(void)
{
  memset(ESP8266_Buffer.Body, 0, USART_RX_BUF_SIZE);  // ��ջ���������
  ESP8266_Buffer.FinishFlag = 0;  // ���FinishFlag, ��ʾû�н������
  ESP8266_Buffer.Length = 0;      // ���Length, ��ʾ���ճ���Ϊ0
}

/**
 * @brief  ����ATָ��
 * @param  cmd ָ���ַ���(����Ҫ\r\n)
 * @param  timeout ��ʱʱ��(��λ:100ms)
 * @retval 0:���ͳɹ� 1:����ʧ��
 */
uint8_t ESP8266_SendCmd(char* cmd, uint8_t timeout) {
  ESP8266_ClearBuffer();
  ESP8266_Printf("%s\r\n", cmd);
  while (timeout--) {
    Delay_ms(100);
    if (strstr(ESP8266_Buffer.Body, "OK"))
      return 0;
  }
  return 1;
}

/**
 * @brief  ��λESP8266ģ��
 * @param  timeout ��ʱʱ��(��λ:100ms)
 * @retval 0:��λ�ɹ� 1:��λʧ��
*/
uint8_t ESP8266_Reset(uint16_t timeout) {
  ESP8266_ClearBuffer();
  ESP8266_RST(RESET);
  Delay_ms(500);
  ESP8266_RST(SET);
  while (timeout--) {
    Delay_ms(100);
    if (strstr(ESP8266_Buffer.Body, "ready"))
      return 0;
  }
  return 1;
}

/**
 * @brief  ���ӷ�����
 * @param  address ��������ַ
 * @param  port �������˿�
 * @param  timeout ��ʱʱ��(��λ:100ms)
 * @retval 0:���ӳɹ� 1:����ʧ�� 2:�����Ѵ��� 3:��ʱ 4:�����ѽ���
 */
uint8_t ESP8266_ConnectServer(char* address, uint16_t port, uint8_t timeout) {
  ESP8266_ClearBuffer();
  ESP8266_Printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", address, port);
  while (timeout--) {
    Delay_ms(100);
    if (strstr(ESP8266_Buffer.Body, "CONNECT")) {
      ESP8266_ClearBuffer();
      ESP8266_Printf("AT+CIPSEND\r\n");
      while (timeout--) {
        Delay_ms(100);
        if (strstr(ESP8266_Buffer.Body, "\r\nOK\r\n\r\n>"))
          return 0;
      }
      return 4;
    }
    if (strstr(ESP8266_Buffer.Body, "CLOSED"))
      return 1;
    if (strstr(ESP8266_Buffer.Body, "ALREADY CONNECTED"))
      return 2;
  }
  return 3;                   //��ʱ���󣬷���3
}

/**
 * @brief  ��ʼ��ESP8266����, ��Ҫʹ�ô��ڽ�ESP8266��ǰ����Ϊ"��������","STAģʽ","������"
 * @param  ESP8266_InitStructure ���ýṹ��
 * @retval  0:�ɹ� 1:TCP�Ѿ����ӣ�������IP
 */
uint8_t ESP8266_InitConnection(ESP8266_InitTypeDef* ESP8266_InitStructure)
{
  // ѭ��ִ��, ֱ�����ӳɹ�
  uint8_t res = 0;
  do{
    #if output_mode
    Serial_Printf("����ESP8266�Ƿ�����\r\n");
    #endif
    res = ESP8266_SendCmd("AT",50);
  }while(res);
  res = 0;
  do{
    #if output_mode
    Serial_Printf("ESP8266����WIFI\r\n");
    #endif
    char temp[100];
    sprintf(temp, "AT+CWJAP=\"%s\",\"%s\"",ESP8266_InitStructure->ESP8266_WIFI_SSID,ESP8266_InitStructure->ESP8266_WIFI_PASSWORD);
    res = ESP8266_SendCmd(temp,50);
  }while(res);
  res = 0;
  do{
    #if output_mode
    Serial_Printf("ESP8266����Ϊ͸��ģʽ\r\n");
    #endif
    res = ESP8266_SendCmd("AT+CIPMODE=1",50);
  }while(res);
  res = 0;
  do {
    // 0:���ӳɹ� 1:����ʧ�� 2:�����Ѵ��� 3:��ʱ 4:�����ѽ���
    #if output_mode
    Serial_Printf("ESP8266���ӷ�����\r\n");
    #endif
    res = ESP8266_ConnectServer(ESP8266_InitStructure->ESP8266_SERVER_ADDRESS, ESP8266_InitStructure->ESP8266_SERVER_PORT, 50);
  } while (res == 3 || res == 1 || res == 4);
  if(res == 2)
  {
    return 1;
  }
  return 0;
}

/**
 * @brief  ��ȡESP8266_Buffer�ṹ���е�����
 * @param  None
 * @retval char* �ַ�����
 */
char* ESP8266_GetBuffer(void) {
  return ESP8266_Buffer.Body;
}

/**
 * @brief  HTTP GET��������
 * @param  address �����ַ
 * @param  timeout ��ʱʱ��(��λ:100ms)
 * @retval char* �ַ�����
 */
char* ESP8266_HTTP_Get(char* address, uint8_t timeout)
{
  #if output_mode
  // Serial_Printf("����HTTP GET����\r\n");
  #endif

  ESP8266_ClearBuffer();
  ESP8266_Printf("GET %s HTTP/1.1\r\n\r\n", address);
  while(timeout--)
  {
    Delay_ms(100);
    if(ESP8266_Buffer.FinishFlag)
    {
      return ESP8266_GetBuffer();
    }
  }
  
  return "HTTP Send Failed.";
}  

/**
 * @brief  HTTP������»�����������
 * @param  ESP8266_HTTP_PostDataTypestructure ���ýṹ��
 * @retval char* �ַ�����
 */
char* ESP8266_HTTPPostData(ESP8266_HTTP_PostDataTypedef* ESP8266_HTTP_PostDataTypestructure)
{
  // ���ȴ��������͵�����
  char data[1000];  // ��������
  ESP8266_HTTP_PostDataTypestructure->address = "/thcp"; // ��ַ
  ESP8266_HTTP_PostDataTypestructure->accessToken = "9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa";
  sprintf(data, "access=%s&temperature=%d.%d&humidity=%d.%d&concentration=%d.%d", ESP8266_HTTP_PostDataTypestructure->accessToken, ESP8266_HTTP_PostDataTypestructure->temperature_H, ESP8266_HTTP_PostDataTypestructure->temperature_L, ESP8266_HTTP_PostDataTypestructure->humidity_H, ESP8266_HTTP_PostDataTypestructure->humidity_L, ESP8266_HTTP_PostDataTypestructure->concentration_H, ESP8266_HTTP_PostDataTypestructure->concentration_L);
  // Serial_Printf("data:%s\n", data);
  uint32_t dataLength = strlen(data);

  // ��������
  ESP8266_ClearBuffer();
  ESP8266_Printf("POST %s HTTP/1.1\r\nContent-Length:%d\r\nContent-Type:application/x-www-form-urlencoded;charset=UTF-8\r\n\r\n%s\r\n\r\n", ESP8266_HTTP_PostDataTypestructure->address, dataLength, data);
  // Serial_Printf("POST %s HTTP/1.1\r\nContent-Length:%d\r\nContent-Type:application/x-www-form-urlencoded;charset=UTF-8\r\n\r\n%s\r\n\r\n", ESP8266_HTTP_PostDataTypestructure->address, dataLength, data);
  while(ESP8266_HTTP_PostDataTypestructure->timeout--)
  {
    Delay_ms(100);
    if(ESP8266_Buffer.FinishFlag)
    {
      return ESP8266_GetBuffer();
    }
  }
  return "HTTP Send Failed.";
}

/**
 * @brief  HTTP�����Ͷ���
 * @param  ESP8266_HTTP_SendSmsTypestructure ���ýṹ��
 * @retval char* �ַ�����
 */
char* ESP8266_HTTPSendSms(ESP8266_HTTP_SendSmsTypedef* ESP8266_HTTP_SendSmsTypestructure)
{
  // ���ȴ��������͵�����
  char data[1000];  // ��������
  ESP8266_HTTP_SendSmsTypestructure->address = "/sendsms"; // ��ַ
  ESP8266_HTTP_SendSmsTypestructure->accessToken = "9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa";
  sprintf(data, "access-token=%s", ESP8266_HTTP_SendSmsTypestructure->accessToken);
  uint32_t dataLength = strlen(data);

  // ��������
  ESP8266_ClearBuffer();
  ESP8266_Printf("POST %s HTTP/1.1\r\nContent-Length:%d\r\nContent-Type:application/x-www-form-urlencoded;charset=UTF-8\r\n\r\n%s\r\n\r\n", ESP8266_HTTP_SendSmsTypestructure->address, dataLength, data);
  while(ESP8266_HTTP_SendSmsTypestructure->timeout--)
  {
    Delay_ms(100);
    if(ESP8266_Buffer.FinishFlag)
    {
      return ESP8266_GetBuffer();
    }
  }
  return "HTTP Send Failed.";
}

/**
 * @brief  ����3���յ������жϻص�����
 * @param  None
 * @retval None
 */
static void ESP8266_FrameFinish_CallBack(void) {
  #if debug_mode
  Serial_Printf("\r\n�жϻص������������:\r\n%s", ESP8266_Buffer.Body);
	#endif
}

/**
 * @brief  ����3�жϺ���
 * @param  None
 * @retval None
 */
void USART3_IRQHandler(void) {
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    if (ESP8266_Buffer.Length < (USART_RX_BUF_SIZE - 1))
    {
      ESP8266_Buffer.Body[ESP8266_Buffer.Length++] = (char)USART_ReceiveData(USART3);
    }
  }
  if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
  {
    ESP8266_Buffer.FinishFlag = 1;
    ESP8266_Buffer.Body[ESP8266_Buffer.Length] = '\0';
    // ������ڿ����жϱ�־λ
    volatile uint16_t temp;
    temp = USART3->SR;
    temp = USART3->DR;
    ESP8266_FrameFinish_CallBack();
  }
}
