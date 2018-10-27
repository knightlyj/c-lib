/********************************************
 实现单个LED的显示效果
 ***************************************
 */
#pragma once

#include "cl_common.h"



#define WDG_FEED_INTERVAL   (7 * 1000)  //喂狗间隔,单位是ms
#define WDG_RESET_TIME      (10)        //WDG复位的时间,单位是秒


extern void IWDG_Init(uint8_t second);
extern void IWDG_Feed(void);
extern void IWDG_Process(void);

