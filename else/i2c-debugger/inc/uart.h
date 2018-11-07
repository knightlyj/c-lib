/********************************************
 实现单个LED的显示效果
 ***************************************
 */
#pragma once

#include "cl_common.h"
#include "cl_queue.h"

enum
{
    Uart1 = 1,
    Uart2 = 2,
    UartMax,
};


CL_QUEUE_DECL(USART1_SendBuffer);

CL_QUEUE_DECL(USART2_SendBuffer);


extern CL_RESULT UART_Init(int uartNum, int baudRate);
extern CL_RESULT UART_Exit(int uartNum);

extern CL_RESULT UART_Send(int uartNum, const uint8_t* buff, uint16_t length, uint16_t* sendBytes);


