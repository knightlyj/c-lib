#pragma once

#include "cl_common.h"
#include "hl_config.h"

//定时器id
typedef enum
{
    HL_Timer2 = 2,
    HL_Timer3 = 3,
    HL_Timer4 = 4,
    HL_Timer5 = 5,
} HL_TimerId_t;

/**
  * 定时器触发方式
  * HL_TimerAutoLoad: 自动装载
  * HL_TimerOneShot:  一次性
  */
typedef enum
{
    HL_TimerAutoLoad = 0,
    HL_TimerOneShot = 1,   //暂时没有实现
} HL_TimerMode_t;


//定时器时钟频率
#define TIMER_CLOCK_FREQ        (8000000ul)


/**
  * @brief  开启定时器
  * @param  id: 要操作的定时器id
  *         mode:   定时器的触发模式
  *         interval: 定时器触发间隔,单位为ms
  * @retval None
  */
extern int HL_TimerStart(HL_TimerId_t id, HL_TimerMode_t mode, uint16_t interval);

/**
  * @brief  关闭定时器
  * @param  id: 要操作的定时器id
  * @retval None
  */
extern int HL_TimerStop(HL_TimerId_t id);


