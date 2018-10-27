#include "helper.h"
#include "global_variables.h"
#include "iwdg.h"
#include "stm32f10x.h"

void DelayMs(uint16_t ms)
{
    uint64_t targetTime;
    targetTime = g_sysTotalTime + ms;
    while(g_sysTotalTime < targetTime)
    {
        IWDG_Feed();
    }
}


void DelayUs(uint16_t us)
{
    int i;
    for(i = 0; i < us; i++)
    {
        __NOP();__NOP();__NOP();__NOP();
    }
}

