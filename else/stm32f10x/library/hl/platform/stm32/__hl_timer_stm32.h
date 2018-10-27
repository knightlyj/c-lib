#include "stm32f10x_tim.h"

//TODO IRQ�ܹ�,OneShotģʽ

//��ʱ��2~5����ͨ��
//����ʱ�� auto loadģʽ
static void __HL_TimerAutoLoad(TIM_TypeDef *timer, uint32_t interval)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    uint16_t arr;
    uint16_t psc = 1000; //Ԥ��Ƶ,�̶��������
    uint32_t periph;

    switch((int)timer)
    {
        case (int)TIM2:
            NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
            periph = RCC_APB1Periph_TIM2;
            break;
        case (int)TIM3:
            NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
            periph = RCC_APB1Periph_TIM3;
            break;
        case (int)TIM4:
            NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
            periph = RCC_APB1Periph_TIM4;
            break;
        case (int)TIM5:
            NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
            periph = RCC_APB1Periph_TIM5;
            break;
        default:
            return;
    }

    //��NVIC�ж�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(periph, ENABLE); //��ʱ��
    arr = TIMER_CLOCK_FREQ / 1000 * interval / psc;

    //����interval�������,���ö�ʱ��
    TIM_TimeBaseStructure.TIM_Period = arr - 1; 
    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(timer, &TIM_TimeBaseStructure);

    //����ʱ���ж�
    TIM_ITConfig(timer,TIM_IT_Update, ENABLE);  

    //ʹ�ܶ�ʱ��
    TIM_Cmd(timer, ENABLE); //使能TIM
}

//�ض�ʱ��
static void __HL_TimerStop(TIM_TypeDef *timer)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    uint32_t periph;
    switch((int)timer)
    {
        case (int)TIM2:
            NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
            periph = RCC_APB1Periph_TIM2;
            break;
        case (int)TIM3:
            NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
            periph = RCC_APB1Periph_TIM3;
            break;
        case (int)TIM4:
            NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
            periph = RCC_APB1Periph_TIM4;
            break;
        case (int)TIM5:
            NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
            periph = RCC_APB1Periph_TIM5;
            break;
        default:
            return;
    }

    TIM_Cmd(timer, DISABLE); //�ض�ʱ��
    TIM_ITConfig(timer,TIM_IT_Update, DISABLE); //�ض�ʱ���ж�
    RCC_APB1PeriphClockCmd(periph, DISABLE); //�ض�ʱ��ʱ��

    //��NVIC�ж�
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}

