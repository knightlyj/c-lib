/********************************************
 实现单个LED的显示效果
 ***************************************
 */
#pragma once

#include "cl_common.h"


#define LED_EFFECT_IDLE         0  
#define LED_EFFECT_WORK         1

//LED 初始化
extern void LED_Init(void);

//LED 退出
extern void LED_Exit(void);

//LED 设置效果
extern void LED_SetEffect(int effect, uint8_t reset);

//LED 处理函数
extern void LED_Process(uint16_t interval);
