#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"

#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_Pin_3

void DS18B20_Init(void);
float DS18B20_GetTemperature(void);
uint16_t DS18B20_GetRawData(void);

#endif
