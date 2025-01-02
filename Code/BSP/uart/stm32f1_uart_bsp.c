
//BSP
#include "systick_bsp.h"
#include "stm32f1_uart_bsp.h"

#ifndef USE_OS
#include "heap4_bsp.h"
#include "ring_buf_bsp.h"
#else
#include "os_bsp.h"
#endif

//Application
#include "priority.h"

static Uart_dev usart1 = NULL;
static Uart_dev usart2 = NULL;
static Uart_dev usart3 = NULL;

static Uart_dev debug_port = NULL;

const Uart_info USART1_PA9TX_PA10RX =
    {
        .USARTx = USART1,

        .tx_port = GPIOA,
        .tx_pin = GPIO_Pin_9,
        .tx_port_rcc = RCC_APB2Periph_GPIOA,

        .rx_port = GPIOA,
        .rx_pin = GPIO_Pin_10,
        .rx_port_rcc = RCC_APB2Periph_GPIOA,
};

const Uart_info USART3_PB10TX_PB11RX =
    {
        .USARTx = USART3,

        .tx_port = GPIOB,
        .tx_pin = GPIO_Pin_10,
        .tx_port_rcc = RCC_APB2Periph_GPIOB,

        .rx_port = GPIOB,
        .rx_pin = GPIO_Pin_11,
        .rx_port_rcc = RCC_APB2Periph_GPIOB,
};

void usartx_send(Uart_dev usartx, uint8_t *data, uint8_t size)
{
    uint8_t i = 0;
    while (i < size)
    {
        USART_SendData(usartx->USARTx, *(data + i));
        i++;
    }
}

/**
 * @return     status code
 *             - 0 success
 *             - 1 failed
 * @note       none
 */
uint8_t usartx_read_1byte(Uart_dev usartx, uint8_t *data)
{
#ifndef USE_OS
    return pop_1byte_from_ringbuffer(&(usartx->buf), data);
#else
    return !os_queue_recv(usartx->rx_queue, data, 0);
#endif
}

/**
 * @return     receive len
 * @note       none
 */
uint8_t usartx_read_buf(Uart_dev usartx, uint8_t *buf)
{
    uint8_t i = 0;

#ifndef USE_OS
    while (1)
    {
        if (pop_1byte_from_ringbuffer(&(usartx->buf), buf + i))
        {
            delay_1ms(1);
            if (is_ringbuffer_empty(&(usartx->buf)))
            {
                break;
            }
        }
        else
        {
            i++;
        }
    }
#else
    TickType_t ticks = os_ms_to_ticks(2);
    while (os_queue_recv(usartx->rx_queue, buf + i, ticks) == osTURE)
    {
        i++;
    }
#endif
    return i;
}

Uart_dev usartx_init(Uart_info dev_info, uint32_t baudval, uint8_t *buf_mem, uint16_t buf_size)
{
#ifndef USE_OS
    ring_buffer_init(&(dev->buf), buf_mem, buf_size);
    Uart_dev dev = (Uart_dev)pvPortMalloc(sizeof(__Uart_dev));
    if (dev == NULL)
    {
        return dev;
    }
#else
    Uart_dev dev = (Uart_dev)os_malloc(sizeof(__Uart_dev));
    dev->rx_queue = os_queue_create(buf_size,1);
    if (dev == NULL)
    {
        return dev;
    }
    
#endif

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    uint8_t NVIC_IRQChannel;
    uint8_t priority;
    

    /* init gpio */
    RCC_APB2PeriphClockCmd(dev_info.rx_port_rcc, ENABLE);
    RCC_APB2PeriphClockCmd(dev_info.tx_port_rcc, ENABLE);

    GPIO_InitStructure.GPIO_Pin = dev_info.tx_pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(dev_info.tx_port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = dev_info.rx_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(dev_info.rx_port, &GPIO_InitStructure);

    dev->USARTx = dev_info.USARTx;
    dev->baudval = baudval;

    switch ((int)(dev_info.USARTx))
    {
    case (int)USART1:
    {
        usart1 = dev;
        NVIC_IRQChannel = USART1_IRQn;
        priority = UASRT1_PRIORITY;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        break;
    }
    case (int)USART2:
    {
        usart2 = dev;
        NVIC_IRQChannel = USART2_IRQn;
        priority = UASRT2_PRIORITY;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        break;
    }
    case (int)USART3:
    {
        usart3 = dev;
        NVIC_IRQChannel = USART3_IRQn;
        priority = UASRT3_PRIORITY;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        break;
    }
    default:
        break;
    }

    NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_BaudRate = baudval;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(dev->USARTx, &USART_InitStructure);
    USART_ITConfig(dev->USARTx, USART_IT_RXNE, ENABLE);

    USART_Cmd(dev->USARTx, ENABLE);

    

    return dev;
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
/* receive data */
#ifndef USE_OS
        push_1byte_to_ringbuffer(&(usart1->buf), USART_ReceiveData(USART1));
#else
        uint8_t cIn;
        cIn = USART_ReceiveData(USART1);
        os_queue_send(usart1->rx_queue, &cIn, 0);
#endif
    }
}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
/* receive data */
#ifndef USE_OS
        push_1byte_to_ringbuffer(&(usart2->buf), USART_ReceiveData(USART2));
#else
        uint8_t cIn;
        cIn = USART_ReceiveData(USART2);
        os_queue_send(usart2->rx_queue, &cIn, 0);
#endif
    }
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
/* receive data */
#ifndef USE_OS
        push_1byte_to_ringbuffer(&(usart3->buf), USART_ReceiveData(USART3));
#else
        uint8_t cIn;
        cIn = USART_ReceiveData(USART3);
        os_queue_send(usart3->rx_queue, &cIn, 0);
#endif
    }
}

void usart_debbug_port_set(Uart_dev usartx)
{
    debug_port = usartx;
}

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

int fputc(int ch, FILE *f)
{
    while ((debug_port->USARTx->SR & 0X40) == 0)
        ; //循环发送,直到发送完毕
    debug_port->USARTx->DR = (u8)ch;
    return ch;
}
