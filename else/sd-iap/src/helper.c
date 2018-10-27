#include "helper.h"
#include "global_variables.h"
#include "iwdg.h"


void DelayMs(uint16_t ms)
{
    uint64_t targetTime;
    targetTime = g_sysTotalTime + ms;
    while(g_sysTotalTime < targetTime)
    {
        IWDG_Feed();
    }
}

