
#include "can2515.h"

#include "Mcp2515_reg.h"
#include "MCP2515.h"
#include "CAR_XXX2515.h"
#include "global_variables.h"

_CAN2515Queue CAN2515Queue;

#define _FIFO_OV_2515
/*
 ******************************************************************
 * 函数功能: 清除通信队列
 * 入口参数: 无
 * 返 回 值: 无
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
 * 函数功能: 判断串口队列是否为空
 * 入口参数:
 * 返 回 值: 1: 空; 0:非空
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
 * 函数功能: 判队列是否满
 * 入口参数:
 * 返 回 值: 1: 满; 0:非满
 *******************************************************************
 */
uint8_t IsFullCAN2515Queue(void)
{
    if( CAN2515Queue.Q_out == (CAN2515Queue.Q_in+1) % MAX_CAN2515_SIZE)             //  尾部插入数据  如果 尾部 + 1 == 头部 表明满
    {
#ifdef _FIFO_OV_2515

        CAN2515Queue.Q_out = (CAN2515Queue.Q_out + 1) % MAX_CAN2515_SIZE;           // 定义了作溢出处理的话，满了，将 out  向前推一个

        return 0;                                                       // 0 表明没满 还有空 BUFFER ，可以往内面再压
#else
        return 1;                                                       // 1 表明满了
#endif
    }
    else
    {
        return 0;
    }
}

/*
 ******************************************************************
 * 函数功能: 将数据插入队列,从 Q_in 位插入
 * 入口参数: element:被插元素
 * 返 回 值: 1: 成功; 0:失败
 *******************************************************************
 */
uint8_t InsertCAN2515Queue(CAN_SPIMessage element)
{
    if(!IsFullCAN2515Queue())                                               // 是否为满
    {
        memcpy(&CAN2515Queue.Elem[CAN2515Queue.Q_in],&element,sizeof(CAN_SPIMessage));

        CAN2515Queue.Q_in = (CAN2515Queue.Q_in + 1) % MAX_CAN2515_SIZE;

        return 1;
    }
    else                                                                // 队列满
    {
        return 0;
    }
}

/*
 *******************************************************************
 * 函数功能: 重新设置队列头指针
 * 入口参数: head: 新头
 * 返 回 值: 无
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
 * 函数功能: 取对头
 * 入口参数: head:对头;*element;数据
 * 返 回 值: 1: 成功 0: 失败
 *******************************************************************
 */
uint8_t GetCAN2515Queue(uint16_t out,CAN_SPIMessage *element)
{
    if(out != CAN2515Queue.Q_in)                                    // 到队列尾
    {
        memcpy(element,&CAN2515Queue.Elem[out],sizeof(CAN_SPIMessage)); // 得到数据

        return 1;
    }
    else
    {
        return 0;                                               // 空队列
    }
}


/*
 *******************************************************************
 * 函数功能: can数据处理
 * 入口参数:
 * 返 回 值:
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
        res = CAN_Receive_MCP2515();                                    // 直接转成了 CAN 的格式
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
            InsertCAN2515Queue(CANRxSPIMassage);                        // 压入堆栈 can_queue.c 用这个
        }
    }
    //队列处理
    TempOut = CAN2515Queue.Q_out;
    if(1 == GetCAN2515Queue(TempOut,&CANRxSPITempMessage))
    {
        TempOut = (TempOut + 1) % MAX_CAN2515_SIZE;                         // 查询头前滚
        SetHeadCAN2515Queue(TempOut);

        Log("id: %d, data: %d\r\n", CANRxSPITempMessage.Id_Add, CANRxSPITempMessage.Data[0]);
        CAN_MCP2515_Transmit(&CANRxSPITempMessage);
    }
    else
    {
        // printf("CAN queue is empty\r\n");
    }


    interval = g_sysTotalTime - lastRecvTime;
    if(interval > 5000)     //总线空闲一定时间后,MCP2515休眠
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

