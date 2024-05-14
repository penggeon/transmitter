#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "delay.h"
#include "usart.h"
#include "ESP8266.h"
#include "Serial.h"
#include <string.h>
#include "OLED.h"
#define debug_mode 0	// 1: 打开调试模式, 0: 关闭调试模式
#define output_mode 1 // 1: 打开正常串口输出, 0: 关闭正常串口输出

USART_Buffer ESP8266_Buffer; // 创建一个USART_Buffer结构体变量

/**
 * @brief  ESP8266模块初始化, 此函数仅仅进行硬件电路初始化
 * @param  None
 * @retval None
 */
void ESP8266_Init(void)
{
  Usart3_Init(115200);      // 串口3初始化
  ESP8266_RST(SET);   // 复位电平上拉, 即模块不复位
  ESP8266_CH_PD(SET); // 使能模块
}

/**
 * @brief  串口发送一个字节
 * @param  Byte 要发送的字节数据
 * @retval None
 */
void ESP8266_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);		// 将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	// 等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
 * @brief  串口发送一个字符串
 * @param  string 要发送字符串的首地址
 * @retval None
 */
void ESP8266_SendString(char *string)
{
  uint32_t i;
	for (i = 0; string[i] != '\0'; i ++)  // 遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		ESP8266_SendByte(string[i]);		    // 依次调用Serial_SendByte发送每个字节数据
	}
}

/**
 * @brief  自己封装的prinf函数
 * @param  format 格式化字符串
 * @param  ... 可变的参数列表
 * @retval None
*/
void ESP8266_Printf(char *format, ...)
{
	char String[1024];      //定义字符数组
	va_list arg;            //定义可变参数列表数据类型的变量arg
	va_start(arg, format);  //从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);    //使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);                      //结束变量arg
	ESP8266_SendString(String);       //串口发送字符数组（字符串）
}

/**
 * @brief  清空ESP8266_Buffer结构体
 * @param  None
 * @retval None
 */
void ESP8266_ClearBuffer(void)
{
  memset(ESP8266_Buffer.Body, 0, USART_RX_BUF_SIZE);  // 清空缓存区数据
  ESP8266_Buffer.FinishFlag = 0;  // 清空FinishFlag, 表示没有接收完成
  ESP8266_Buffer.Length = 0;      // 清空Length, 表示接收长度为0
}

/**
 * @brief  发送AT指令
 * @param  cmd 指令字符串(不需要\r\n)
 * @param  timeout 超时时间(单位:100ms)
 * @retval 0:发送成功 1:发送失败
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
 * @brief  复位ESP8266模块
 * @param  timeout 超时时间(单位:100ms)
 * @retval 0:复位成功 1:复位失败
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
 * @brief  连接服务器
 * @param  address 服务器地址
 * @param  port 服务器端口
 * @param  timeout 超时时间(单位:100ms)
 * @retval 0:连接成功 1:连接失败 2:连接已存在 3:超时 4:连接已建立
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
  return 3;                   //超时错误，返回3
}

/**
 * @brief  初始化ESP8266连接, 需要使用串口将ESP8266提前设置为"开启回显","STA模式","单连接"
 * @param  ESP8266_InitStructure 配置结构体
 * @retval  0:成功 1:TCP已经连接，请重置IP
 */
uint8_t ESP8266_InitConnection(ESP8266_InitTypeDef* ESP8266_InitStructure)
{
  // 循环执行, 直到连接成功
  uint8_t res = 0;
  do{
    #if output_mode
    Serial_Printf("测试ESP8266是否连接\r\n");
    #endif
    res = ESP8266_SendCmd("AT",50);
  }while(res);
  res = 0;
  do{
    #if output_mode
    Serial_Printf("ESP8266连接WIFI\r\n");
    #endif
    char temp[100];
    sprintf(temp, "AT+CWJAP=\"%s\",\"%s\"",ESP8266_InitStructure->ESP8266_WIFI_SSID,ESP8266_InitStructure->ESP8266_WIFI_PASSWORD);
    res = ESP8266_SendCmd(temp,50);
  }while(res);
  res = 0;
  do{
    #if output_mode
    Serial_Printf("ESP8266设置为透传模式\r\n");
    #endif
    res = ESP8266_SendCmd("AT+CIPMODE=1",50);
  }while(res);
  res = 0;
  do {
    // 0:连接成功 1:连接失败 2:连接已存在 3:超时 4:连接已建立
    #if output_mode
    Serial_Printf("ESP8266连接服务器\r\n");
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
 * @brief  获取ESP8266_Buffer结构体中的数据
 * @param  None
 * @retval char* 字符数组
 */
char* ESP8266_GetBuffer(void) {
  return ESP8266_Buffer.Body;
}

/**
 * @brief  HTTP GET类型请求
 * @param  address 请求地址
 * @param  timeout 超时时间(单位:100ms)
 * @retval char* 字符数组
 */
char* ESP8266_HTTP_Get(char* address, uint8_t timeout)
{
  #if output_mode
  // Serial_Printf("发送HTTP GET请求\r\n");
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
 * @brief  HTTP请求更新机房环境数据
 * @param  ESP8266_HTTP_PostDataTypestructure 配置结构体
 * @retval char* 字符数组
 */
char* ESP8266_HTTPPostData(ESP8266_HTTP_PostDataTypedef* ESP8266_HTTP_PostDataTypestructure)
{
  // 首先处理即将发送的数据
  char data[1000];  // 请求数据
  ESP8266_HTTP_PostDataTypestructure->address = "/thcp"; // 地址
  ESP8266_HTTP_PostDataTypestructure->accessToken = "9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa";
  sprintf(data, "access=%s&temperature=%d.%d&humidity=%d.%d&concentration=%d.%d", ESP8266_HTTP_PostDataTypestructure->accessToken, ESP8266_HTTP_PostDataTypestructure->temperature_H, ESP8266_HTTP_PostDataTypestructure->temperature_L, ESP8266_HTTP_PostDataTypestructure->humidity_H, ESP8266_HTTP_PostDataTypestructure->humidity_L, ESP8266_HTTP_PostDataTypestructure->concentration_H, ESP8266_HTTP_PostDataTypestructure->concentration_L);
  // Serial_Printf("data:%s\n", data);
  uint32_t dataLength = strlen(data);

  // 发送数据
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
 * @brief  HTTP请求发送短信
 * @param  ESP8266_HTTP_SendSmsTypestructure 配置结构体
 * @retval char* 字符数组
 */
char* ESP8266_HTTPSendSms(ESP8266_HTTP_SendSmsTypedef* ESP8266_HTTP_SendSmsTypestructure)
{
  // 首先处理即将发送的数据
  char data[1000];  // 请求数据
  ESP8266_HTTP_SendSmsTypestructure->address = "/sendsms"; // 地址
  ESP8266_HTTP_SendSmsTypestructure->accessToken = "9mzdDx3K3pgmGdkTcq2ONsUKMp3VSTkIxoUwnHk7bljH4GrkYSeJXFtBp8HYN4f0DJAYPa";
  sprintf(data, "access-token=%s", ESP8266_HTTP_SendSmsTypestructure->accessToken);
  uint32_t dataLength = strlen(data);

  // 发送数据
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
 * @brief  串口3接收到数据中断回调函数
 * @param  None
 * @retval None
 */
static void ESP8266_FrameFinish_CallBack(void) {
  #if debug_mode
  Serial_Printf("\r\n中断回调函数串口输出:\r\n%s", ESP8266_Buffer.Body);
	#endif
}

/**
 * @brief  串口3中断函数
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
    // 清除串口空闲中断标志位
    volatile uint16_t temp;
    temp = USART3->SR;
    temp = USART3->DR;
    ESP8266_FrameFinish_CallBack();
  }
}
