
#ifndef CAN2515_QUEUE_H_
#define CAN2515_QUEUE_H_

#define MAX_CAN2515_SIZE 50

#include "mcp2515.h"

typedef struct 
{
	CAN_SPIMessage Elem[MAX_CAN2515_SIZE];
	uint16_t Q_out;		// 头部取出数
	uint16_t Q_in;		// 尾部插入数据
}_CAN2515Queue;

extern _CAN2515Queue CAN2515Queue;

void ClearCan2515Queue(void);
uint8_t IsEmptyCAN2515Queue(void);
uint8_t IsFullCAN2515Queue(void);
uint8_t InsertCAN2515Queue(CAN_SPIMessage element);
void SetHeadCAN2515Queue(uint16_t out);
uint8_t GetCAN2515Queue(uint16_t out,CAN_SPIMessage *element);
void cCAN2515ReceiveProcess(void);
#endif
