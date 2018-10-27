/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f10x_usart.h"

#include "cl_queue.h"
#include "app_upgrade.h"
#include "led.h"
#include "global_variables.h"


CL_QUEUE_DEF_INIT(USART1_SendBuffer, 1024, uint8_t, );

//
int fputc(int ch, FILE *f)
{
    uint8_t data = ch;
    int res;

    res = CL_QueueAdd(&USART1_SendBuffer, &data);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    if(res == CL_SUCCESS)
    {
        return ch;
    }
    else
    {
        return EOF;
    }

}

void USART1_IRQHandler(void)
{
    uint8_t data;
    uint16_t c;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        c = USART_ReceiveData(USART1);
    }

    else if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_TXE);
        if(CL_QueuePoll(&USART1_SendBuffer, &data) == CL_SUCCESS)
        {
            USART_SendData(USART1, data);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }

    if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        c = USART_ReceiveData(USART1); 
    }
}

void TIM2_IRQHandler(void)
{

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //��־�ж�
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //���ж�
        g_sysTotalTime++;

        LED_Process(1);
    }

}

//void TIM3_IRQHandler(void)
//{
//
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //�?�? TIM3 更新中断发生与否
//    {
//        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除 TIM3 更新中断标志
//
//    }
//
//}
//
//void TIM4_IRQHandler(void)
//{
//
//    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
//    {
//        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//        g_sysTotalTime++;
//    }
//
//}
//
//void TIM5_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
//    {
//        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
//
//    }
//}

//void EXTI15_10_IRQHandler(void)
//{
//    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
//    {
//        EXTI_ClearITPendingBit(EXTI_Line10); //清除中断标志�?
//    }
//}

