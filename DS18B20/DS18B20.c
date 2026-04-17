#include "DS18B20.h"
#include "Delay.h"

#define DS18B20_MODE_INPUT  0
#define DS18B20_MODE_OUTPUT 1

static void DS18B20_Mode(uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;

    if(mode == DS18B20_MODE_OUTPUT)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    }
    else
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    }

    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

static void DS18B20_High(void)
{
    DS18B20_Mode(DS18B20_MODE_OUTPUT);
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
}

static void DS18B20_Low(void)
{
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
    DS18B20_Mode(DS18B20_MODE_OUTPUT);
}

static uint8_t DS18B20_WaitPresence(void)
{
    uint8_t presence;
    uint16_t retry = 0;

    DS18B20_Mode(DS18B20_MODE_INPUT);

    while(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN) && retry < 200)
    {
        retry++;
        Delay_us(1);
    }

    if(retry >= 200)
        return 0;
    else
        retry = 0;

    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN) && retry < 240)
    {
        retry++;
        Delay_us(1);
    }

    presence = (retry < 240) ? 1 : 0;

    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN));

    DS18B20_Mode(DS18B20_MODE_OUTPUT);
    DS18B20_High();

    return presence;
}

static void DS18B20_WriteBit(uint8_t bit)
{
    if(bit)
    {
        DS18B20_Low();
        Delay_us(6);
        DS18B20_High();
        Delay_us(64);
    }
    else
    {
        DS18B20_Low();
        Delay_us(60);
        DS18B20_High();
        Delay_us(10);
    }
}

static uint8_t DS18B20_ReadBit(void)
{
    uint8_t bit;

    DS18B20_Low();
    Delay_us(2);
    DS18B20_High();
    DS18B20_Mode(DS18B20_MODE_INPUT);
    Delay_us(12);

    bit = GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN);

    Delay_us(50);
    DS18B20_Mode(DS18B20_MODE_OUTPUT);
    DS18B20_High();

    return bit;
}

static uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, byte = 0;

    for(i = 0; i < 8; i++)
    {
        byte >>= 1;
        if(DS18B20_ReadBit())
        {
            byte |= 0x80;
        }
    }

    return byte;
}

static void DS18B20_WriteByte(uint8_t byte)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        DS18B20_WriteBit(byte & 0x01);
        byte >>= 1;
    }
}

void DS18B20_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    DS18B20_High();
    Delay_ms(10);
}

float DS18B20_GetTemperature(void)
{
    uint8_t temp_low, temp_high;
    int16_t temp;
    float temperature;

    DS18B20_Low();
    Delay_us(480);
    DS18B20_High();
    Delay_us(70);

    if(!DS18B20_WaitPresence())
    {
        return -999.0f;
    }

    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);

    DS18B20_Low();
    Delay_us(480);
    DS18B20_High();
    Delay_us(70);

    if(!DS18B20_WaitPresence())
    {
        return -999.0f;
    }

    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);

    temp_low = DS18B20_ReadByte();
    temp_high = DS18B20_ReadByte();

    temp = (int16_t)((temp_high << 8) | temp_low);

    if(temp == 0)
    {
        temperature = 0.0f;
    }
    else if(temp & 0x8000)
    {
        temp = ~temp + 1;
        temperature = -(float)temp * 0.0625f;
    }
    else
    {
        temperature = (float)temp * 0.0625f;
    }

    return temperature;
}

uint16_t DS18B20_GetRawData(void)
{
    uint8_t temp_low, temp_high;

    DS18B20_Low();
    Delay_us(480);
    DS18B20_High();
    Delay_us(70);

    if(!DS18B20_WaitPresence())
    {
        return 0xFFFF;
    }

    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);

    DS18B20_Low();
    Delay_us(480);
    DS18B20_High();
    Delay_us(70);

    if(!DS18B20_WaitPresence())
    {
        return 0xFFFF;
    }

    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);

    temp_low = DS18B20_ReadByte();
    temp_high = DS18B20_ReadByte();

    return (uint16_t)((temp_high << 8) | temp_low);
}
