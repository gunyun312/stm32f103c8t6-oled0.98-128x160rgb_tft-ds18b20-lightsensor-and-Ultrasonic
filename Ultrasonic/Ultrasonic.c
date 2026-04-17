#include "stm32f10x.h"
#include "Ultrasonic.h"
#include "delay.h"

#define delay_ms Delay_ms
#define delay_us Delay_us

void Ultrasonic_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TRIG_LOW;
}

float Ultrasonic_GetDistance(void)
{
    u32 t = 0;
    u32 timeout = 0;
    float distance = 0;
    u32 start_time = 0;
    u32 end_time = 0;

    TRIG_HIGH;
    delay_us(20);
    TRIG_LOW;

    // 等待 ECHO 变为高电平，添加超时
    timeout = 0;
    while(!ECHO_READ) {
        timeout++;
        if(timeout > 100000) {
            return 0;
        }
    }
    
    // 记录开始时间（使用简单的循环计数，更可靠）
    t = 0;
    while(ECHO_READ) {
        t++;
        if(t > 500000) // 增加超时值
        {
            return 0;
        }
    }
    
    // 校准后的距离计算
    // 基于实际测量数据校准：6cm显示10cm，10cm显示16cm
    distance = (float)t / 96.0f; // 校准系数

    return distance;
}
