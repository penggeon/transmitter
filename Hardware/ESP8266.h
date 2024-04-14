#ifndef __ESP8266_H
#define __ESP8266_H

#define USART_RX_BUF_SIZE 10240 // USART接收缓冲区大小
typedef struct {
  char Body[USART_RX_BUF_SIZE];
  uint16_t Length     :15;
  uint16_t FinishFlag :1;
} USART_Buffer; // USART接收缓冲区结构体
typedef struct {
  char* ESP8266_WIFI_SSID;        // WiFi名称
  char* ESP8266_WIFI_PASSWORD;    // WiFi密码
  char* ESP8266_SERVER_ADDRESS;   // 服务器地址
  uint16_t ESP8266_SERVER_PORT;   // 服务器端口
} ESP8266_InitTypeDef;
typedef struct {
  char* address;
  char* accessToken;
  uint8_t temperature_H;
  uint8_t temperature_L;
  uint8_t humidity_H;
  uint8_t humidity_L;
  uint16_t concentration_H;
  uint16_t concentration_L;
  uint8_t timeout;
} ESP8266_HTTP_PostDataTypedef;
typedef struct {
  char* address;
  char* accessToken;
  uint8_t timeout;
} ESP8266_HTTP_SendSmsTypedef;

#define ESP8266_RST(state)   GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)state)
#define ESP8266_CH_PD(state) GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)state)


void ESP8266_Init(void);
void ESP8266_SendString(char *string);
void ESP8266_Printf(char *format, ...);
void ESP8266_CheckDevice(void);
uint8_t ESP8266_SendCmd(char* cmd, uint8_t timeout);
uint8_t ESP8266_Reset(uint16_t timeout);
uint8_t ESP8266_InitConnection(ESP8266_InitTypeDef* ESP8266_InitStructure);
char* ESP8266_GetBuffer(void);
char* ESP8266_HTTP_Get(char* address, uint8_t timeout);
char* ESP8266_HTTPPostData(ESP8266_HTTP_PostDataTypedef* ESP8266_HTTP_PostDataTypestructure);
char* ESP8266_HTTPSendSms(ESP8266_HTTP_SendSmsTypedef* ESP8266_HTTP_SendSmsTypestructure);
uint8_t ESP8266_ConnectServer(char* address, uint16_t port, uint8_t timeout);

#endif
