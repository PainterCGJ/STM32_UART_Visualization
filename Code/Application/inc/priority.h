#pragma once

typedef enum
{
    OS_CALL_MAX_PRIORITY = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
    UASRT1_PRIORITY,
    UASRT2_PRIORITY,
    UASRT3_PRIORITY,
} Interrupt_Priority;

typedef enum
{
    MINIMAL_TASK_PRIORITY = 0,
    COM_TASK_PRIORITY,
} Task_Priority;
