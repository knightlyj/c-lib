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
#include "mcp2515.h"
#include "can2515.h"

const uint8_t strInitDone[] = "init done\r\n";

CAN_SPIMessage msg;
extern uint8_t uartRecv;

int main(void)
{
    int i = 0;
    int a;
    static uint64_t lastPrintTime = 0;
    uint64_t interval;
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

    MCP2515_Init();

    Log("%s", strInitDone);

    ClearCan2515Queue();

    

    while (1)
    {
//        IWDG_Process();
        cCAN2515ReceiveProcess();

        IWDG_Feed();
        interval = g_sysTotalTime - lastPrintTime;
        if(interval > 1000)
        {
            lastPrintTime = g_sysTotalTime;
            a = i % 256;
            msg.Id_Add = a;
            msg.DLC = 8;
            msg.Data[0] = a;
            i++;

            Log("1s\r\n");
            CAN_MCP2515_Transmit(&msg);

        }

        if(uartRecv)
        {
            uartRecv = 0;
            cFWWakeMCP2515();
        }
        
    }
}


