/*
 * Serial.h
 * 串口通信功能头文件
 * 基于STM32F103C8T6芯片
 */

#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"   // STM32F10x系列芯片头文件
#include <stdio.h>        // 标准输入输出头文件，用于printf函数
#include <stdarg.h>       // 可变参数头文件，用于Serial_Printf函数

/**
 * @brief 串口初始化函数
 * @param 无
 * @retval 无
 * @note 配置USART1，波特率9600，8位数据位，1位停止位，无校验位
 */
void Serial_Init(void);

/**
 * @brief 发送单个字节
 * @param Byte 要发送的字节
 * @retval 无
 */
void Serial_SendByte(uint8_t Byte);

/**
 * @brief 发送数组
 * @param Array 要发送的数组指针
 * @param Length 数组长度
 * @retval 无
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length);

/**
 * @brief 发送字符串
 * @param String 要发送的字符串指针
 * @retval 无
 */
void Serial_SendString(char *String);

/**
 * @brief 发送数字
 * @param Number 要发送的数字
 * @param Length 数字位数
 * @retval 无
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length);

/**
 * @brief 串口printf函数
 * @param format 格式化字符串
 * @param ... 可变参数
 * @retval 无
 */
void Serial_Printf(char *format, ...);

/**
 * @brief 接收数据变量
 * @note 存储从串口接收到的单个字节数据
 */
extern uint8_t Serial_RxData;

/**
 * @brief 接收完成标志
 * @note 当接收到数据时，此标志置1，用户处理完数据后应手动清零
 */
extern uint8_t Serial_RxFlag;
uint8_t SeriGetRxFlag(void);
uint8_t SeriGetRxData(void);

/**
 * @brief 按顺序接收三个字节数据并赋值给x、y、z
 * @param x 第一个字节的指针
 * @param y 第二个字节的指针
 * @param z 第三个字节的指针
 * @retval 无
 * @note 阻塞等待，直到接收到三个字节的数据
 */
 void Serial_ReceiveXY(uint8_t *x, uint8_t *y);
void Serial_ReceiveXYZ(uint8_t *x, uint8_t *y, uint8_t *z);
#endif
