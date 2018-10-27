#include "stm32f10x_tim.h"

//TODO IRQ架构,OneShot模式

//定时器2~5测试通过
//开定时器 auto load模式
static void __HL_TimerAutoLoad(TIM_TypeDef *timer, uint32_t interval)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    uint16_t arr;
    uint16_t psc = 1000; //预分频,固定用这个数
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

    //开NVIC中断
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(periph, ENABLE); //开时钟
    arr = TIMER_CLOCK_FREQ / 1000 * interval / psc;

    //根据interval计算参数,配置定时器
    TIM_TimeBaseStructure.TIM_Period = arr - 1; 
    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(timer, &TIM_TimeBaseStructure);

    //开定时器中断
    TIM_ITConfig(timer,TIM_IT_Update, ENABLE);  

    //使能定时器
    TIM_Cmd(timer, ENABLE); //浣胯兘TIM
}

//关定时器
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

    TIM_Cmd(timer, DISABLE); //关定时器
    TIM_ITConfig(timer,TIM_IT_Update, DISABLE); //关定时器中断
    RCC_APB1PeriphClockCmd(periph, DISABLE); //关定时器时钟

    //关NVIC中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}

