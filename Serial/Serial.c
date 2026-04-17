/*
 * Serial.c
 * 串口通信功能实现
 * 基于STM32F103C8T6芯片
 */

#include "stm32f10x.h"   // STM32F10x系列芯片头文件
#include "Serial.h"       // 自定义串口头文件

#define USART1_DR_Base 0x40013804  // USART1数据寄存器地址

/**
 * @brief 串口初始化函数
 * @param 无
 * @retval 无
 * @note 配置USART1，波特率9600，8位数据位，1位停止位，无校验位
 */
void Serial_Init(void)
{
    // 使能USART1和GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置PA9为复用推挽输出（USART1_TX）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          // PA9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置PA10为上拉输入（USART1_RX）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;         // PA10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART1参数
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;                          // 波特率9600
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;    // 同时使能发送和接收
    USART_InitStructure.USART_Parity = USART_Parity_No;                // 无校验位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;             // 1位停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;        // 8位数据位
    USART_Init(USART1, &USART_InitStructure);
    
    // 使能USART1接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // 配置NVIC优先级分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // 配置USART1中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                  // USART1中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    // 使能中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                 // 子优先级1
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief 发送单个字节
 * @param Byte 要发送的字节
 * @retval 无
 */
void Serial_SendByte(uint8_t Byte)
{
    // 发送数据
    USART_SendData(USART1, Byte);
    // 等待发送完成（TXE标志置位）
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
 * @brief 发送数组
 * @param Array 要发送的数组指针
 * @param Length 数组长度
 * @retval 无
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(Array[i]);  // 逐个字节发送
    }
}

/**
 * @brief 发送字符串
 * @param String 要发送的字符串指针
 * @retval 无
 */
void Serial_SendString(char *String)
{
    uint8_t i = 0;
    while (String[i] != '\0')  // 直到遇到字符串结束符
    {
        Serial_SendByte(String[i]);  // 逐个字符发送
        i++;
    }
}

/**
 * @brief 计算幂函数
 * @param X 底数
 * @param Y 指数
 * @retval 计算结果
 */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)  // 循环Y次
    {
        Result *= X;  // 累乘
    }
    return Result;
}

/**
 * @brief 发送数字
 * @param Number 要发送的数字
 * @param Length 数字位数
 * @retval 无
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        // 计算每一位数字并发送
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief 重定向fputc函数，支持printf函数
 * @param ch 要发送的字符
 * @param f 文件指针
 * @retval 发送的字符
 */
int fputc(int ch, FILE *f)
{
    Serial_SendByte(ch);  // 调用串口发送函数
    return ch;
}

/**
 * @brief 串口printf函数
 * @param format 格式化字符串
 * @param ... 可变参数
 * @retval 无
 */
void Serial_Printf(char *format, ...)
{
    char String[100];  // 定义缓冲区
    va_list arg;       // 可变参数列表
    va_start(arg, format);  // 初始化可变参数
    vsprintf(String, format, arg);  // 格式化输出到缓冲区
    va_end(arg);  // 结束可变参数
    Serial_SendString(String);  // 发送格式化后的字符串
}

// 全局变量
uint8_t Serial_RxData;  // 接收数据
uint8_t Serial_RxFlag;  // 接收完成标志

uint8_t SeriGetRxFlag(void)
{
	if(Serial_RxFlag == 1)
	{
		Serial_RxFlag=0;
		return 1;
	}
	return 0;
}
uint8_t SeriGetRxData(void)
{
	
	return Serial_RxData;
}

/**
 * @brief 按顺序接收三个字节数据并赋值给x、y、z
 * @param x 第一个字节的指针
 * @param y 第二个字节的指针
 * @param z 第三个字节的指针
 * @retval 无
 * @note 阻塞等待，直到接收到三个字节的数据
 */
void Serial_ReceiveXYZ(uint8_t *x, uint8_t *y, uint8_t *z)
{
    // 等待接收第一个字节
    while (SeriGetRxFlag() == 0);
    *x = SeriGetRxData();
    
    // 等待接收第二个字节
    while (SeriGetRxFlag() == 0);
    *y = SeriGetRxData();
    
    // 等待接收第三个字节
    while (SeriGetRxFlag() == 0);
    *z = SeriGetRxData();
}
void Serial_ReceiveXY(uint8_t *x, uint8_t *y)
{
    // 等待接收第一个字节
    while (SeriGetRxFlag() == 0);
    *x = SeriGetRxData();
    
    // 等待接收第二个字节
    while (SeriGetRxFlag() == 0);
    *y = SeriGetRxData();
    
   
   
}

/**
 * @brief USART1中断处理函数
 * @param 无
 * @retval 无
 */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)  // 检查接收中断标志
    {
        Serial_RxData = USART_ReceiveData(USART1);  // 读取接收数据
        Serial_RxFlag = 1;  // 设置接收完成标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // 清除中断标志
    }
}