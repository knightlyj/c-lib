
#include "can2515.h"

#include "Mcp2515_reg.h"
#include "MCP2515.h"
#include "CAR_XXX2515.h"
#include "global_variables.h"

_CAN2515Queue CAN2515Queue;

#define _FIFO_OV_2515
/*
 ******************************************************************
 * ��������: ���ͨ�Ŷ���
 * ��ڲ���: ��
 * �� �� ֵ: ��
 ******************************************************************
 */
void ClearCan2515Queue(void)
{
    int i;

    for(i = 0; i < MAX_CAN2515_SIZE; i++)
    {
        memset(&CAN2515Queue.Elem[i],0,sizeof(CAN_SPIMessage));
    }

    CAN2515Queue.Q_out = 0;
    CAN2515Queue.Q_in = 0;
}

/*
 ******************************************************************
 * ��������: �жϴ��ڶ����Ƿ�Ϊ��
 * ��ڲ���:
 * �� �� ֵ: 1: ��; 0:�ǿ�
 *******************************************************************
 */
uint8_t IsEmptyCAN2515Queue(void)
{
    if(CAN2515Queue.Q_out == CAN2515Queue.Q_in)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 ******************************************************************
 * ��������: �ж����Ƿ���
 * ��ڲ���:
 * �� �� ֵ: 1: ��; 0:����
 *******************************************************************
 */
uint8_t IsFullCAN2515Queue(void)
{
    if( CAN2515Queue.Q_out == (CAN2515Queue.Q_in+1) % MAX_CAN2515_SIZE)             //  β����������  ��� β�� + 1 == ͷ�� ������
    {
#ifdef _FIFO_OV_2515

        CAN2515Queue.Q_out = (CAN2515Queue.Q_out + 1) % MAX_CAN2515_SIZE;           // ���������������Ļ������ˣ��� out  ��ǰ��һ��

        return 0;                                                       // 0 ����û�� ���п� BUFFER ��������������ѹ
#else
        return 1;                                                       // 1 ��������
#endif
    }
    else
    {
        return 0;
    }
}

/*
 ******************************************************************
 * ��������: �����ݲ������,�� Q_in λ����
 * ��ڲ���: element:����Ԫ��
 * �� �� ֵ: 1: �ɹ�; 0:ʧ��
 *******************************************************************
 */
uint8_t InsertCAN2515Queue(CAN_SPIMessage element)
{
    if(!IsFullCAN2515Queue())                                               // �Ƿ�Ϊ��
    {
        memcpy(&CAN2515Queue.Elem[CAN2515Queue.Q_in],&element,sizeof(CAN_SPIMessage));

        CAN2515Queue.Q_in = (CAN2515Queue.Q_in + 1) % MAX_CAN2515_SIZE;

        return 1;
    }
    else                                                                // ������
    {
        return 0;
    }
}

/*
 *******************************************************************
 * ��������: �������ö���ͷָ��
 * ��ڲ���: head: ��ͷ
 * �� �� ֵ: ��
 *******************************************************************
 */
void SetHeadCAN2515Queue(uint16_t out)
{
    if(CAN2515Queue.Q_out != CAN2515Queue.Q_in)
    {
        CAN2515Queue.Q_out = out;
    }
}

/*
 ******************************************************************
 * ��������: ȡ��ͷ
 * ��ڲ���: head:��ͷ;*element;����
 * �� �� ֵ: 1: �ɹ� 0: ʧ��
 *******************************************************************
 */
uint8_t GetCAN2515Queue(uint16_t out,CAN_SPIMessage *element)
{
    if(out != CAN2515Queue.Q_in)                                    // ������β
    {
        memcpy(element,&CAN2515Queue.Elem[out],sizeof(CAN_SPIMessage)); // �õ�����

        return 1;
    }
    else
    {
        return 0;                                               // �ն���
    }
}


/*
 *******************************************************************
 * ��������: can���ݴ���
 * ��ڲ���:
 * �� �� ֵ:
 *******************************************************************
 */
void cCAN2515ReceiveProcess(void)
{
    uint16_t TempOut;
    static uint64_t lastRecvTime = 0;
    uint64_t interval;
    uint8_t res;
    static uint8_t mcp2515Sleep = 0;
    
    if(ucMCP2515RevMark)
    {
        ucMCP2515RevMark = 0;
        res = CAN_Receive_MCP2515();                                    // ֱ��ת���� CAN �ĸ�ʽ
        if(res == 1)
        {
            lastRecvTime = g_sysTotalTime;
            mcp2515Sleep = 0;
            Log("mcp2515 wake up\r\n");
        }
        else if(res == 2)
        {
            Log("can recv\r\n");
            lastRecvTime = g_sysTotalTime;
            InsertCAN2515Queue(CANRxSPIMassage);                        // ѹ���ջ can_queue.c �����
        }
    }
    //���д���
    TempOut = CAN2515Queue.Q_out;
    if(1 == GetCAN2515Queue(TempOut,&CANRxSPITempMessage))
    {
        TempOut = (TempOut + 1) % MAX_CAN2515_SIZE;                         // ��ѯͷǰ��
        SetHeadCAN2515Queue(TempOut);

        Log("id: %d, data: %d\r\n", CANRxSPITempMessage.Id_Add, CANRxSPITempMessage.Data[0]);
        CAN_MCP2515_Transmit(&CANRxSPITempMessage);
    }
    else
    {
        // printf("CAN queue is empty\r\n");
    }


    interval = g_sysTotalTime - lastRecvTime;
    if(interval > 5000)     //���߿���һ��ʱ���,MCP2515����
    {
        if(mcp2515Sleep == 0)
        {
            cSetMCP2515ToSleepMode();
            mcp2515Sleep = 1;
            Log("mcp2515 sleep\r\n");
        }
    }
}


/*************************************************************************/

