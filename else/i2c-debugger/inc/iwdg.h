/********************************************
 ʵ�ֵ���LED����ʾЧ��
 ***************************************
 */
#pragma once

#include "cl_common.h"



#define WDG_FEED_INTERVAL   (7 * 1000)  //ι�����,��λ��ms
#define WDG_RESET_TIME      (10)        //WDG��λ��ʱ��,��λ����


extern void IWDG_Init(uint8_t second);
extern void IWDG_Feed(void);
extern void IWDG_Process(void);

