#include "hl_timer.h"

#if HL_MCU_TYPE == HL_STM32F10X_CL
#include "platform/stm32/__hl_timer_stm32.h"

#endif


int HL_TimerStart(HL_TimerId_t id, HL_TimerMode_t mode, uint16_t interval)
{
#if HL_MCU_TYPE == HL_STM32F10X_CL
    TIM_TypeDef* TIM;
    switch(id)
    {
        case HL_Timer2:
            TIM = TIM2;
            break;
        case HL_Timer3:
            TIM = TIM3;
            break;
        case HL_Timer4:
            TIM = TIM4;
            break;
        case HL_Timer5:
            TIM = TIM5;
            break;
        default:
            return CL_INVALID_PARAM;
    }

    switch(mode)
    {
        case HL_TimerAutoLoad:
            __HL_TimerAutoLoad(TIM, interval);
            break;
        case HL_TimerOneShot:
            return CL_INVALID_PARAM;
    }

    return CL_SUCCESS;
#endif
}


int HL_TimerStop(HL_TimerId_t id)
{
#if HL_MCU_TYPE == HL_STM32F10X_CL
    TIM_TypeDef* TIM;
    switch(id)
    {
        case HL_Timer2:
            TIM = TIM2;
            break;
        case HL_Timer3:
            TIM = TIM3;
            break;
        case HL_Timer4:
            TIM = TIM4;
            break;
        case HL_Timer5:
            TIM = TIM5;
            break;
        default:
            return CL_INVALID_PARAM;
    }

    __HL_TimerStop(TIM);

    return CL_SUCCESS;
#endif

}




