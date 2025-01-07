#pragma once

#include "stm32f10x.h"
#include "stdint.h"
#include "stdio.h"

#ifndef USE_OS
#include "ring_buf_bsp.h"
#else
#include "os_bsp.h"
#endif

typedef struct
{
    USART_TypeDef *USARTx;

    GPIO_TypeDef *tx_port;
    uint32_t tx_pin;
    uint32_t tx_port_rcc;

    GPIO_TypeDef *rx_port;
    uint32_t rx_pin;
    uint32_t rx_port_rcc;

} Uart_info;

typedef struct
{
    USART_TypeDef *USARTx;
    uint32_t baudval;
#ifndef USE_OS
    Ringbuffer_t buf;
#else
    OS_Queue rx_queue;
#endif

} __Uart_dev;

typedef __Uart_dev *Uart_dev;

/* info list */
extern const Uart_info USART1_PA9TX_PA10RX;
extern const Uart_info USART3_PB10TX_PB11RX;

Uart_dev usartx_init(Uart_info init_info, uint32_t baudval, uint8_t *rx_buf_mem, uint16_t rx_buf_size);
uint8_t usartx_read_1byte(Uart_dev usartx, uint8_t *data);
uint8_t usartx_read_buf(Uart_dev usartx, uint8_t *buf);
void usartx_send(Uart_dev usartx, uint8_t *data, uint8_t size);
void usart_debbug_port_set(Uart_dev usartx);
