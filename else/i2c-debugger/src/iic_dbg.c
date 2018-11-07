#include "iic_dbg.h"
#include "cl_queue.h"
#include "gpio_iic.h"
#include "uart.h"
#include "helper.h"
#include "24cxx.h"
#include "global_variables.h"

#define IIC_DBG_READ    1
#define IIC_DBG_WRITE   2

typedef struct
{
    uint8_t taskType;
    uint8_t addr;
    uint8_t reg;
    uint8_t data;
} IIC_DebugTask_t;

static void UploadData(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint16_t sendBytes;
    uint8_t buff[5];
    buff[0] = 0xe1;
    buff[1] = 0xe2;
    buff[2] = addr;
    buff[3] = reg;
    buff[4] = data;
    UART_Send(Uart2, buff, CL_ARRAY_LENGTH(buff), &sendBytes);
}

static uint64_t lastRecvTime = 0;
static uint8_t recvBuff[128];
static uint8_t recvCnt;
void IIC_DebugRecvByte(uint8_t data)
{   
    if(recvCnt < CL_ARRAY_LENGTH(recvBuff))
        recvBuff[recvCnt++] = data;
    lastRecvTime = g_sysTotalTime;
}

void IIC_DebugProcess(void)
{
    IIC_DebugTask_t task;

//    int i;
    
    if(recvCnt >= 4 && (g_sysTotalTime - lastRecvTime > 10))
    {
//        for(i = 0; i < recvCnt; i++)
//        {
//            Log("0x%02x, ", recvBuff[i]);
//        }
//        Log("\r\n");
    
        USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        if(recvBuff[0] != 0xe1)
        {
            Log("head error\r\n");
            goto out;
        }
        
        memset(&task, 0, sizeof(task));
        if(recvBuff[1] == 0xe2)
        {   //read
            task.taskType = IIC_DBG_READ;
        }
        else if(recvBuff[1] == 0xe3)
        {   //write
            task.taskType = IIC_DBG_WRITE;
        }
        else
        {   //error
            Log("cmd error\r\n");
            goto out;
        }


        task.addr = recvBuff[2]; //TODO: 地址最低位,根据读写操作,可能要处理
        task.reg = recvBuff[3];

        if(task.taskType == IIC_DBG_READ)
        {
            task.data = AT24CXX_ReadOneByte(task.reg); 
            UploadData(task.addr, task.reg, task.data);
            Log("read at 0x%02x:0x%02x\r\n", task.addr, task.reg);
        }
        else if(task.taskType == IIC_DBG_WRITE)
        {
            if(recvCnt >= 5)
            {
                task.data = recvBuff[4];
                AT24CXX_WriteOneByte(task.reg, task.data);
                Log("write 0x%02x to 0x%02x:0x%02x\r\n", task.data, task.addr, task.reg);
            }
        }

out:
        recvCnt = 0;
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
}


void IIC_DebugInit(void)
{
    IIC_Init();
}

void IIC_DebugExit(void)
{   //暂时不做处理

}





