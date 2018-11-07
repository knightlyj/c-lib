#pragma once

#include "stm32f10x.h"

//SDA
#define IIC_SDA_PORT_GROUP      GPIOB
#define IIC_SDA_PORT_NUM        GPIO_Pin_13

//SCL
#define IIC_SCL_PORT_GROUP      GPIOB
#define IIC_SCL_PORT_NUM        GPIO_Pin_12

// IIC���в�������
void IIC_Init(void);                        // ��ʼ��IIC��IO��                
void IIC_Start(void);                       // ����IIC��ʼ�ź�
void IIC_Stop(void);                        // ����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);                 // IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);        // IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void);                      // IIC�ȴ�ACK�ź�
void IIC_Ack(void);                         // IIC����ACK�ź�
void IIC_NAck(void);                        // IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);   










