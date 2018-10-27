#ifndef __MCP2515_H
#define __MCP2515_H

#include "cl_common.h"


#define MCP2515_OSC 				8000000 //16M

#define _500K_ 0
#define _250K_ 1
#define _125K_ 2
#define _100K_ 3
#define _50K_  4 
#define _33_3K_ 5

#define MCP2515_CAN_FREQ_VALUE      _500K_

//根据不同总线频率,设置对应的分频
#if MCP2515_CAN_FREQ_VALUE == _500K_        
#define MCP2515_BRP 				1     // (500/freq)*2-1
#elif MCP2515_CAN_FREQ_VALUE == _250K_
#define MCP2515_BRP 				3
#elif MCP2515_CAN_FREQ_VALUE == _125K_
#define MCP2515_BRP 				7
#elif MCP2515_CAN_FREQ_VALUE == _100K_
#define MCP2515_BRP 				9
#elif MCP2515_CAN_FREQ_VALUE == _50K_
#define MCP2515_BRP 				19
#elif MCP2515_CAN_FREQ_VALUE == _33_3K_
#define MCP2515_BRP 				29
#endif

//通过上面的分频来匹配不同频率,不用修改这些段的宽度
#define MCP2515_SYNC                1       //同步段宽度,不能修改
#define MCP2515_SJW 				1       //0~3 
#define MCP2515_PHSEG				1       //0~7
#define MCP2515_PRSEG				1       //0~7


#define _MCP2515_CAN_STD			0	// 标准帧
#define _MCP2515_CAN_EXT			1	// 扩展帧

#define _MCP2515_CAN_RTR_DATA		0	// 数据帧
#define _MCP2515_CAN_RTR_REMOTE 	1	// 远程帧
//CAN message structure (按照 CAN协议)		
typedef struct
{															
	uint32_t Id_Add; 				   // 桢地址								
	uint8_t	Ext;						// 是否为扩展格式						 
	uint8_t	Remote; 					// 是否为远程桢 						 
	uint8_t	DLC;						// 字节数								 
	uint8_t	Data[8];											  
	//u8 Priority;					// 用于桢优先级设置00--03四个优先级 00--low
}CAN_SPIMessage;														  
extern CAN_SPIMessage CANRxSPIMassage;						
extern CAN_SPIMessage CANTxSPIMessage;
extern CAN_SPIMessage CANRxSPITempMessage;

// RXB0 			这样设置后 RXB0 不接收数据
#define _MCP2515_M0_FAMT		_MCP2515_CAN_STD					// 这样配置后 RXB0 只接受 ID = 0 的报文，也就是 RXB0 不用于接收   
#define _MCP2515_M0_ID			0x7FF

#define _MCP2515_ID1			0x00								// 但总线上是没有 ID = 0 的报文,所以 RXB0 这路是没用到
#define _MCP2515_ID1_FAMT		_MCP2515_CAN_STD

#define _MCP2515_ID2			0x00
#define _MCP2515_ID2_FAMT		_MCP2515_CAN_STD





extern uint8_t ucMCP2515RevMark;

void cBSP_SPI_InitConfig(void);
void EXTI0_IRQHandler(void);
uint8_t CAN_MCP2515MessagePending(void); 								 // 查询方式。判断是有效的CAN信息 FIFO0
uint8_t CAN_Receive_MCP2515(void);
void CAN_MCP2515_Transmit(CAN_SPIMessage * Message);
void cSetMCP2515ToSleepMode(void);

extern void MCP2515_Init(void);
extern void MCP2515_Exit(void);
extern void MCP2515_ErrorCheck(void);
extern void cFWWakeMCP2515(void);

#endif
/***************************************************************/
