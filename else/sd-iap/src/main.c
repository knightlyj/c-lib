/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "stm32f10x_conf.h"
#include "stm32f10x.h"


#include "hl_timer.h"
#include "cl_queue.h"
#include "app_upgrade.h"
#include "code_protect.h"
#include "led.h"
#include "uart.h"
#include "global_variables.h"
#include "flash_layout.h"
#include "iwdg.h"
#include "board_config.h"
#include "helper.h"

const uint8_t strInitDone[] = "init done\r\n";

int main(void)
{
//    int i;
    IWDG_Init(10);

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);

    HL_TimerStart(HL_Timer2, HL_TimerAutoLoad, 1);

    UART_Init(Uart1, 38400);

    if(check() == CL_FALSE)
    {
        while(1);
    }

    LED_Init();
    LED_SetEffect(LED_EFFECT_IDLE, 0);
    AppUpgrade_Init();
    
    Log("%s", strInitDone);

    Log("app addr: %x\r\n", APP_START_ADDR);
    
    while (1)
    {
        AppUpgrade_Process();
        IWDG_Process();

        if(g_jumpToApp == CL_TRUE)
        {
           
            Log("jump to app\r\n");
            DelayMs(500);

            IWDG_ReloadCounter(); //跳转之前喂狗一次 
            //关闭已开启的功能
            AppUpgrade_Exit();
            LED_Exit();
            UART_Exit(Uart1);
            HL_TimerStop(HL_Timer2);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, DISABLE);
            
            //jump to app
            JumpToAddr(APP_START_ADDR);
            
        }
        
    }
}


