/********************************************
 ʵ�ֵ���LED����ʾЧ��
 ***************************************
 */
#pragma once

#include "cl_common.h"


enum
{
    Uart1 = 1,
    UartMax,
};


extern CL_RESULT UART_Init(int uartNum, int baudRate);

extern CL_RESULT UART_Exit(int uartNum);



