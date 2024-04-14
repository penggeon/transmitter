#ifndef __UTILS_H
#define __UTILS_H
#include "stm32f10x.h"                  // Device header

typedef struct {
  uint8_t Humi_H;
  uint8_t Humi_L;
  uint8_t Humi_H_Threshold;
  uint8_t Humi_L_Threshold;
} Humidity_Typedef;     // 湿度结构体
typedef struct {
  uint8_t Temp_H;
  uint8_t Temp_L;
  uint8_t Temp_H_Threshold;
  uint8_t Temp_L_Threshold;
} Temperature_Typedef;  // 温度结构体
typedef struct {
  uint16_t Concentration_H;
  uint16_t Concentration_L;
  uint16_t Concentration_H_Threshold;
  uint16_t Concentration_L_Threshold;
} Concentration_Typedef;  // 浓度结构体

uint8_t Utils_IsOverThreshold(Humidity_Typedef* Humi, Temperature_Typedef* Temp, Concentration_Typedef* Concentration);
void Utils_ShowWelcomeMessage(void);
void Utils_GetInformationFromResJson_8(char* res_json, char* name, uint8_t* integer_part, uint8_t* decimal_part);
void Utils_GetInformationFromResJson_16(char* res_json, char* name, uint16_t* integer_part, uint16_t* decimal_part);

#endif
