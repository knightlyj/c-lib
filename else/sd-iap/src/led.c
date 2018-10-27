#include "led.h"

#include "stm32f10x.h"
#include "board_config.h"


CL_INLINE static void LED_Work()
{
    GPIO_ResetBits(LED_PORT_GROUP, LED_PORT_NUM);
}

CL_INLINE static void LED_Stop()
{
    GPIO_SetBits(LED_PORT_GROUP, LED_PORT_NUM);
}

typedef struct
{
    const uint32_t* effect;     //��ʾЧ������
    const uint16_t length;      //���鳤��
    uint8_t loop;               //�Ƿ�ѭ��
} LedEffect_t;

typedef struct
{
    uint8_t index;      //��ǰЧ�����������
    uint32_t time;      //��ǰ�����Ѿ�������ʱ��
} LedEffectSta_t;



//LED��ʾЧ��,��˳����������ĳ���ʱ��
const static uint32_t idleEffect[] = {1000, 1000};
const static uint32_t workEffect[] = {100, 100};


const static LedEffect_t effects[] = {
                                        {idleEffect, CL_ARRAY_LENGTH(idleEffect), 1},   //0: Idle
                                        {workEffect, CL_ARRAY_LENGTH(workEffect), 1},   //1: Work
                                    }; 

static LedEffectSta_t effStatus = {0, 0};
static int curEffect = LED_EFFECT_IDLE;

void LED_SetEffect(int effect, uint8_t reset)
{
    if(effect >= CL_ARRAY_LENGTH(effects))
        return;
    
    if((curEffect != effect) || reset)
    {
        effStatus.index = 0;
        effStatus.time = 0;
        curEffect = effect;
    }
}


void LED_Process(uint16_t interval)
{
    const LedEffect_t* pEffect;

    if(curEffect >= CL_ARRAY_LENGTH(effects))
    {   //��������
        LED_SetEffect(LED_EFFECT_IDLE, 1);  
    }
    
    pEffect = &effects[curEffect];    
    if(effStatus.index >= pEffect->length)
    {   //��������
        LED_SetEffect(curEffect, 1);
    }

    //��ǰ״̬�ļ�ʱ����
    effStatus.time += interval;
    if(effStatus.time > pEffect->effect[effStatus.index])
    {
        effStatus.time = 0;
        effStatus.index++;
        if(effStatus.index >= pEffect->length)
        {       
            if(pEffect->loop)
            {
                effStatus.index = 0;
            }
            else
            {
                LED_SetEffect(LED_EFFECT_IDLE, 0);  //��ѭ����ʾ,Ч����ʾ��֮��,����idle
            }
        }
    }

    //����index,����������
    if(effStatus.index & 0x01)
    {   //LED ON
        LED_Work();
    }
    else
    {   //LED OFF
        LED_Stop();
    }
}



void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* led */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LED_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT_GROUP, &GPIO_InitStructure);

    LED_Work();
}

void LED_Exit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* led */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = LED_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT_GROUP, &GPIO_InitStructure);
}







