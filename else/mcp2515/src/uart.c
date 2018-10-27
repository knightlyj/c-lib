/* Includes ------------------------------------------------------------------*/
#include "uart.h"

#include "stm32f10x_usart.h"

CL_RESULT UART_Init(int uartNum, int baudRate)
{
//    USART_TypeDef* uart;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if(uartNum == Uart1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

        /* Configure USART Tx as alternate function push-pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* Configure USART Rx as input floating */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudRate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        /* USART configuration */
        USART_Init(USART1, &USART_InitStructure);

        /* Enable USARTy Receive and Transmit interrupts */
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

        /* Enable the USART1 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        /* Enable USART */
        USART_Cmd(USART1, ENABLE);
    }
    return CL_SUCCESS;
}

CL_RESULT UART_Exit(int uartNum)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    if(uartNum == Uart1)
    {
        USART_Cmd(USART1, DISABLE);

        /* Enable the USART1 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_InitStructure);

        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);

    }
    return CL_SUCCESS;
}



