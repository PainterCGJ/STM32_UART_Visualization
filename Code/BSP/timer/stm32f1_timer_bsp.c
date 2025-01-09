#include "stm32f1_timer_bsp.h"

#ifndef USE_OS

#else
#include "os_bsp.h"
#endif

Timer_Dev timer_init()
{
    Timer_Dev dev;
}