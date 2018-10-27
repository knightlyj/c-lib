#include "iwdg.h"
#include "stm32f10x_iwdg.h"
#include "global_variables.h"
#include "stdio.h"


void IWDG_Init(uint8_t second)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //①使能对寄存器 I 写操作
    IWDG_SetPrescaler(IWDG_Prescaler_256); //②设置 IWDG 预分频值:设置 IWDG 预分频值
    IWDG_SetReload(second * 170); //②设置 IWDG 重装载值,这个值大概是1秒,不是非常准
    IWDG_ReloadCounter(); //③按照 IWDG 重装载寄存器的值重装载 IWDG 计数器
    IWDG_Enable(); //④使能 IWDG
    
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();//reload
}

void IWDG_Process(void)
{
    static uint64_t lastFeedTime = 0;
    uint64_t interval = g_sysTotalTime - lastFeedTime;

    if(interval > WDG_FEED_INTERVAL)
    {   //达到喂狗间隔时间,喂狗
        lastFeedTime = g_sysTotalTime;
        IWDG_Feed();
//        Log("定时喂狗\r\n");
    }
}



