#include "stm32f1_i2c_bsp.h"
#ifdef USE_OS
#include "os_bsp.h"
#else

#endif

const I2C_Info I2C1_PB7SDA_PB6SCL_info =
    {
        .I2Cx = I2C1,
        .SDA_port = GPIOB,
        .SDA_rcc = RCC_APB2Periph_GPIOB,
        .SDA_pin = GPIO_Pin_7,

        .SCL_port = GPIOB,
        .SCL_pin = GPIO_Pin_6,
        .SCL_rcc = RCC_APB2Periph_GPIOB,
};

const I2C_Info I2C2_PB11SDA_PB10SCL_info =
    {
        .I2Cx = I2C2,
        .SDA_port = GPIOB,
        .SDA_pin = GPIO_Pin_11,
        .SDA_rcc = RCC_APB2Periph_GPIOB,

        .SCL_port = GPIOB,
        .SCL_pin = GPIO_Pin_10,
        .SCL_rcc = RCC_APB2Periph_GPIOB,
};

I2C_Dev i2c_dev_init(I2C_Info dev_info, uint32_t clock_speed, uint16_t master_addr, uint16_t I2C_Ack, uint16_t slave_addr_len)
{
#ifdef USE_OS
    I2C_Dev dev = (I2C_Dev)os_malloc(sizeof(__I2C_Dev));
#else

#endif
GPIO_InitTypeDef GPIO_InitStruct;

    /* GOIO initialize */
	RCC_APB2PeriphClockCmd(dev_info.SDA_rcc|dev_info.SCL_rcc,ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = dev_info.SDA_pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(dev_info.SDA_port,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = dev_info.SCL_pin;
	GPIO_Init(dev_info.SCL_port,&GPIO_InitStruct);

    dev->dev_info = dev_info;
    dev->clock_speed = clock_speed;
    dev->master_addr = master_addr;
    dev->I2C_Ack = I2C_Ack;
    dev->slave_addr_len = slave_addr_len;

    return dev;
}
