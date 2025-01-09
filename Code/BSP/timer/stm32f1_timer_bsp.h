#pragma once

#include "stm32f10x.h"
#include "stdint.h"
#include "stdio.h"

typedef struct 
{

    void(* callback)(void* parg);
}__Timer_Dev;

typedef __Timer_Dev* Timer_Dev;
