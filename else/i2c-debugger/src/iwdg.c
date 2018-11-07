#include "iwdg.h"
#include "stm32f10x_iwdg.h"
#include "global_variables.h"
#include "stdio.h"


void IWDG_Init(uint8_t second)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //��ʹ�ܶԼĴ��� I д����
    IWDG_SetPrescaler(IWDG_Prescaler_256); //������ IWDG Ԥ��Ƶֵ:���� IWDG Ԥ��Ƶֵ
    IWDG_SetReload(second * 170); //������ IWDG ��װ��ֵ,���ֵ�����1��,���Ƿǳ�׼
    IWDG_ReloadCounter(); //�۰��� IWDG ��װ�ؼĴ�����ֵ��װ�� IWDG ������
    IWDG_Enable(); //��ʹ�� IWDG
    
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();//reload
}

void IWDG_Process(void)
{
    static uint64_t lastFeedTime = 0;
    uint64_t interval = g_sysTotalTime - lastFeedTime;

    if(interval > WDG_FEED_INTERVAL)
    {   //�ﵽι�����ʱ��,ι��
        lastFeedTime = g_sysTotalTime;
        IWDG_Feed();
//        Log("��ʱι��\r\n");
    }
}



