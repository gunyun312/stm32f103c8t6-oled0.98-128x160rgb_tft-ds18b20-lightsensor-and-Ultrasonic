#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#include "stm32f10x.h"

#define TRIG_PIN  GPIO_Pin_2
#define ECHO_PIN  GPIO_Pin_1
#define TRIG_PORT GPIOA
#define ECHO_PORT GPIOA

#define TRIG_HIGH GPIO_SetBits(TRIG_PORT, TRIG_PIN)
#define TRIG_LOW  GPIO_ResetBits(TRIG_PORT, TRIG_PIN)
#define ECHO_READ GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN)

void Ultrasonic_Init(void);
float Ultrasonic_GetDistance(void);

#endif
