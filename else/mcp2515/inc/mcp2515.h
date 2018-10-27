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

//���ݲ�ͬ����Ƶ��,���ö�Ӧ�ķ�Ƶ
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

//ͨ������ķ�Ƶ��ƥ�䲻ͬƵ��,�����޸���Щ�εĿ��
#define MCP2515_SYNC                1       //ͬ���ο��,�����޸�
#define MCP2515_SJW 				1       //0~3 
#define MCP2515_PHSEG				1       //0~7
#define MCP2515_PRSEG				1       //0~7


#define _MCP2515_CAN_STD			0	// ��׼֡
#define _MCP2515_CAN_EXT			1	// ��չ֡

#define _MCP2515_CAN_RTR_DATA		0	// ����֡
#define _MCP2515_CAN_RTR_REMOTE 	1	// Զ��֡
//CAN message structure (���� CANЭ��)		
typedef struct
{															
	uint32_t Id_Add; 				   // ���ַ								
	uint8_t	Ext;						// �Ƿ�Ϊ��չ��ʽ						 
	uint8_t	Remote; 					// �Ƿ�ΪԶ���� 						 
	uint8_t	DLC;						// �ֽ���								 
	uint8_t	Data[8];											  
	//u8 Priority;					// ���������ȼ�����00--03�ĸ����ȼ� 00--low
}CAN_SPIMessage;														  
extern CAN_SPIMessage CANRxSPIMassage;						
extern CAN_SPIMessage CANTxSPIMessage;
extern CAN_SPIMessage CANRxSPITempMessage;

// RXB0 			�������ú� RXB0 ����������
#define _MCP2515_M0_FAMT		_MCP2515_CAN_STD					// �������ú� RXB0 ֻ���� ID = 0 �ı��ģ�Ҳ���� RXB0 �����ڽ���   
#define _MCP2515_M0_ID			0x7FF

#define _MCP2515_ID1			0x00								// ����������û�� ID = 0 �ı���,���� RXB0 ��·��û�õ�
#define _MCP2515_ID1_FAMT		_MCP2515_CAN_STD

#define _MCP2515_ID2			0x00
#define _MCP2515_ID2_FAMT		_MCP2515_CAN_STD





extern uint8_t ucMCP2515RevMark;

void cBSP_SPI_InitConfig(void);
void EXTI0_IRQHandler(void);
uint8_t CAN_MCP2515MessagePending(void); 								 // ��ѯ��ʽ���ж�����Ч��CAN��Ϣ FIFO0
uint8_t CAN_Receive_MCP2515(void);
void CAN_MCP2515_Transmit(CAN_SPIMessage * Message);
void cSetMCP2515ToSleepMode(void);

extern void MCP2515_Init(void);
extern void MCP2515_Exit(void);
extern void MCP2515_ErrorCheck(void);
extern void cFWWakeMCP2515(void);

#endif
/***************************************************************/
