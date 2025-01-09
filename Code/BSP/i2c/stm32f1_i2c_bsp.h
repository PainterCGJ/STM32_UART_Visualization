#pragma once
#include "stdio.h"
#include "stdint.h"
#include "stm32f10x.h"

typedef struct
{
    I2C_TypeDef *I2Cx;

    GPIO_TypeDef *SDA_port;
    uint16_t SDA_pin;
    uint32_t SDA_rcc;

    GPIO_TypeDef *SCL_port;
    uint16_t SCL_pin;
    uint32_t SCL_rcc;

} I2C_Info;

typedef struct
{
    I2C_Info dev_info;

    I2C_TypeDef *I2Cx;
    uint32_t clock_speed;
    uint16_t master_addr;
    uint16_t I2C_Ack;
    uint16_t slave_addr_len;
} __I2C_Dev;

typedef __I2C_Dev *I2C_Dev;
