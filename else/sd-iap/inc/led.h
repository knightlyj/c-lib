/********************************************
 ʵ�ֵ���LED����ʾЧ��
 ***************************************
 */
#pragma once

#include "cl_common.h"


#define LED_EFFECT_IDLE         0  
#define LED_EFFECT_WORK         1

//LED ��ʼ��
extern void LED_Init(void);

//LED �˳�
extern void LED_Exit(void);

//LED ����Ч��
extern void LED_SetEffect(int effect, uint8_t reset);

//LED ������
extern void LED_Process(uint16_t interval);
