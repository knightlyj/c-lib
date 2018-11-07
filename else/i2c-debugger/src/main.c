/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "stm32f10x_conf.h"
#include "stm32f10x.h"


#include "hl_timer.h"
#include "code_protect.h"
#include "uart.h"
#include "global_variables.h"
#include "iwdg.h"
#include "board_config.h"
#include "helper.h"

#include "iic_dbg.h"

const uint8_t strInitDone[] = "init done\r\n";
int main(void)
{
    uint64_t lastPrintTime = 0;
    IWDG_Init(10);
    
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);

    HL_TimerStart(HL_Timer2, HL_TimerAutoLoad, 1);

    UART_Init(Uart1, 9600);
    UART_Init(Uart2, 9600);

    IIC_DebugInit();

    if(check() == CL_FALSE)
    {
        while(1);
    }

    Log("%s", strInitDone);

    while (1)
    {
        IWDG_Process();
        IIC_DebugProcess();

        if(g_sysTotalTime - lastPrintTime > 1000)
        {
            lastPrintTime = g_sysTotalTime;
//            Log("1s\r\n");
        }
    }
}


