#include "stm32f1_i2c_bsp.h"
#ifdef USE_OS
#include "os_bsp.h"
#else

#endif

static I2C_Dev i2c1 = NULL;
static I2C_Dev i2c2 = NULL;

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

I2C_Dev i2c_master_dev_init(I2C_Info dev_info, uint32_t clock_speed, uint16_t master_addr, uint16_t I2C_Ack, uint16_t slave_addr_len)
{
#ifdef USE_OS
    I2C_Dev dev = (I2C_Dev)os_malloc(sizeof(__I2C_Dev));
#else

#endif
    if (dev == NULL)
    {
        return NULL;
    }

    dev->dev_info = dev_info;
    dev->clock_speed = clock_speed;
    dev->master_addr = master_addr;
    dev->I2C_Ack = I2C_Ack;
    dev->slave_addr_len = slave_addr_len;

    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;

    /* GOIO initialize */
    RCC_APB2PeriphClockCmd(dev_info.SDA_rcc | dev_info.SCL_rcc, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Pin = dev_info.SDA_pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(dev_info.SDA_port, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = dev_info.SCL_pin;
    GPIO_Init(dev_info.SCL_port, &GPIO_InitStruct);

    switch ((int)(dev_info.I2Cx))
    {
    case (int)(I2C1):
    {
        i2c1 = dev;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        break;
    }
    case (int)(I2C2):
    {
        i2c2 = dev;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        break;
    }
    }

    I2C_InitStruct.I2C_Ack = I2C_Ack;                             //使能应答
    I2C_InitStruct.I2C_AcknowledgedAddress = dev->slave_addr_len; //IIC 7 位寻址
    I2C_InitStruct.I2C_ClockSpeed = dev->clock_speed;             //通信速率 400k
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;               //占空比 1/2
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;                       //选择IIC模式
    I2C_InitStruct.I2C_OwnAddress1 = dev->master_addr;            //输入主机地址，与从机有所区别即可正常通信
    // I2C_AcknowledgeConfig(dev->dev_info.I2Cx, DISABLE );
    I2C_Init(dev->dev_info.I2Cx, &I2C_InitStruct);
    I2C_Cmd(dev->dev_info.I2Cx, ENABLE);

    return dev;
}
