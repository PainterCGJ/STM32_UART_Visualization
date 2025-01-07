
#include "main.h"

void com_task(void *parg)
{
    Uart_dev com = usartx_init(USART1_PA9TX_PA10RX, 115200, NULL, 100);
    usart_debbug_port_set(com);
    uint8_t cin;
    while (1)
    {
        while(!usartx_read_1byte(com,&cin))
        {
            usartx_send(com,&cin,1);
        }
        
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    systick_init();

    OS_thread_info thread[] = {
        {
            .name = "com_task",
            .param = NULL,
            .priority = COM_TASK_PRIORITY,
            .stack_depth = 255,
            .thread_func = com_task,
        },
    };
    os_start_scheduler(thread, sizeof(thread) / sizeof(OS_thread_info));
    
}

void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName)
{
}
