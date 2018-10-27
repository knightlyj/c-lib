#include "cl_common.h"
#include "mcp2515.h"
#include "mcp2515_reg.h"
#include "board_config.h"
#include "helper.h"
#include "CAR_XXX2515.H"
#include "can2515.h"

//------------CS-------------------
CL_INLINE static void CS_Low()
{
    GPIO_ResetBits(MCP2515_CS_PORT_GROUP, MCP2515_CS_PORT_NUM);
}

CL_INLINE static void CS_High()
{
    GPIO_SetBits(MCP2515_CS_PORT_GROUP, MCP2515_CS_PORT_NUM);
}

//-------------MOSI---------------------
CL_INLINE static void MOSI_Low()
{
    GPIO_ResetBits(MCP2515_MOSI_PORT_GROUP, MCP2515_MOSI_PORT_NUM);
}

CL_INLINE static void MOSI_High()
{
    GPIO_SetBits(MCP2515_MOSI_PORT_GROUP, MCP2515_MOSI_PORT_NUM);
}

//-------------CLK-------------------
CL_INLINE static void CLK_Low()
{
    GPIO_ResetBits(MCP2515_CLK_PORT_GROUP, MCP2515_CLK_PORT_NUM);
}

CL_INLINE static void CLK_High()
{
    GPIO_SetBits(MCP2515_CLK_PORT_GROUP, MCP2515_CLK_PORT_NUM);
}

//---------------MISO-------------------
CL_INLINE static uint8_t Read_MISO()
{
    return GPIO_ReadInputDataBit(MCP2515_MISO_PORT_GROUP, MCP2515_MISO_PORT_NUM);
}


CAN_SPIMessage CANRxSPIMassage; 					 
CAN_SPIMessage CANTxSPIMessage;
CAN_SPIMessage CANRxSPITempMessage;						
u8 ucMCP2515RevMark = 0;

//�ⲿ�ж� 10-15 �������
void EXTI15_10_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line12) != RESET)						//�ж��жϷ�����Դ
	{
	    EXTI_ClearITPendingBit(EXTI_Line12); 							//��� LINE0 �ϵ��жϱ�־λ
		ucMCP2515RevMark = 1;										// ���ñ�־
    	
	}	 
	
//    fAlarmRTCInt = FALSE;
}



/*-------------------------------------------------------------------------------
--�������ƣ�void SPISendByte(unsigned char dat)
--�������ܣ����ģ��SPIʱ��ͨ��SPIдһ���ֽ�����
--���������unsigned char dat��Ҫͨ��SPI���͵�����
--�����������
--˵	���� CS	--\_______________________________________________/--------\___
			 SCK00 ___/---\___/---\____/---\___/---\___/---\___/----\____
			 SI   --\_MSB_/-------\________/-------\___LSB_/--------
			 SO   ----------����̬----------------------------------
			 SCK��00ģʽ�£�SI��SCK��������д�룬��λ��ǰ��
			 ������ʱ��Ƶ��С��10Mʱ�ο�����ģ��4M��
			 CS����ʱ��(CS���͵�SCK��������)��С��50ns
			 CS����ʱ��(CS���ߵ�����ǰSCK��������)��С��50ns
			 CS��ֹʱ��(CS���ߵ���һ������)��С:50ns
			 ���ݽ��������ݱ���ʱ�䶼��10ns
			 CLK�������½������2us

			 �����������CS=0,�����Խ��ŷ������ݣ�������ʾ
			 SPISendByte����
			 SPISendByte����
			 SPISendByte����
			 CS=1;//���������CS
--------------------------------------------------------------------------------*/
void SPISendByte(unsigned char dat)
{
	unsigned char i;
	CS_Low();							// CS�͵�ƽ��Ч�������ⲿ����д����ֽ�����
//	delay_us(50);					// CS����ʱ�䣬��С50ns��������50us
	for(i=0;i<8;i++)
	{
		CLK_Low();					// �͵�ƽʱ�����������ݱ仯(Ϊ��������׼��)��������MCP2515��������
//		delay_us(5);		 
		if(dat&0x80)				// ʹ��0x80������Ϊ��λ�ȷ���
		    MOSI_High(); 				// �������������
		else
			MOSI_Low();
//		delay_us(5);				// ���ݽ���ʱ�䣬����10ns���ɣ�������5us
		CLK_High();
		dat=dat<<1;
//		delay_us(5);				// ���ݱ���ʱ�䣬����10ns���ɣ�������5us
	}
}

/*-------------------------------------------------------------------------------
--�������ƣ�unsigned char SPIRecvByte(void)
--�������ܣ����ģ��SPIʱ��ͨ��SPI����һ���ֽ�����
--�����������
--�������������һ���ֽڵĽ�������
--˵	���� CS	--\_______________________________________________/--------\___
			 SCK-----\___/---\____/---\___/---\___/---\___/----\____
			 SO   -----\___MSB__/-------\__
			 SI   ----------����״̬-------
			 SCK���½��ؽ�������
--------------------------------------------------------------------------------*/
unsigned char SPIRecvByte(void)
{
	unsigned char dat=0;				// ���յ�����
	unsigned char i;
	CS_Low();								// ����������CSΪ�͵�ƽ
	for(i=0;i<8;i++)
	{
		dat=dat<<1; 					// ���ƣ�SO�Ƚ��յ����Ǹ�λ
		CLK_High();
//		delay_us(5);					// SCK�ߵ�ƽά��ʱ�����45ns,������5us
		CLK_Low();						// �½���
//		delay_us(5);					// ά�����ݵĲ���
		if(Read_MISO())	dat=dat|0x01;			// ���һλ��1
		else	dat=dat&0xfe;			// ���һλ��0
	}
	CLK_Low();							// SCK�������
	return dat;
}
/*-------------------------------------------------------------------------------
--�������ƣ�void MCP2515SetRegister(unsigned char address,unsigned char value)
--�������ܣ�����MCP2515�Ĵ�����дһ�ֽ�����
--���������unsigned char address����Ҫ���õļĴ����ĵ�ַ
			unsigned char value  ����Ҫ���õľ���ֵ
--�����������
--˵	���� 
--------------------------------------------------------------------------------*/
void MCP2515SetRegister(unsigned char address,unsigned char value)
{
	CS_High(); 								// �ڵ�����������ʱ��CSҪ������
//	delay_us(5);							// CS��ֹʱ����С50ns,������5us
	SPISendByte(MCP_WRITE); 				// �ֽ�д����0x02,���һ�ֽڵ�ַ��һ�ֽ�����
	SPISendByte(address);
	SPISendByte(value); 					// ����SPIд
	CS_High();
//	delay_us(5);							// CS��ֹʱ����С50ns,������5us
}

/*-------------------------------------------------------------------------------
--�������ƣ�void MCP2515SetRegisterS(unsigned char address,unsigned char values[],
									 unsigned char n)
--�������ܣ�����MCP2515���ͻ�������д����Ĵ���
--���������unsigned char address����Ҫ���õļĴ����ĵ�ַ
			unsigned char values[]����Ҫ���õĻ�������ͷָ��
			unsigned char n
--�����������
--˵	���� 
--------------------------------------------------------------------------------*/
void MCP2515SetRegisterS(unsigned char address,unsigned char values[],unsigned char n)
{
	unsigned char i;
	CS_High();									// ����֮ǰ��֮��CSҪ����
//	delay_us(5);							// CS��ֹʱ����С50ns,������5us
	SPISendByte(MCP_WRITE); 				// ����д����0x02,�����ַ�ֽڣ������ֽ�
	SPISendByte(address);					// Ŀ����ʼ��ַ
	for(i=0;i<n;i++)
		SPISendByte(values[i]);
	CS_High();
//	delay_us(5);							// CS��ֹʱ����С50ns,������5us
}

/*-------------------------------------------------------------------------------
--�������ƣ�unsigned char MCP2515ReadRegister(unsigned char address)
--�������ܣ���ȡָ���Ĵ�����ֵ
--���������unsigned char address����Ҫ��ȡ�ļĴ����ĵ�ַ
--�������������ֵΪ��ȡ�Ľ��
--˵	���� 
--------------------------------------------------------------------------------*/
unsigned char MCP2515ReadRegister(unsigned char address)
{
	unsigned char ret;
	CS_High();
//	delay_us(5);							// ��ʼ���βCS��1��������ѡ�У�CS��ֹʱ����С50ns,������5us
	SPISendByte(MCP_READ);					// �����0x03,�����ַ�ֽڣ�������SI�Ϸ�������
	SPISendByte(address);					// �Ĵ�����ַ
	ret=SPIRecvByte();
	CS_High();
	return ret;
}

/*-------------------------------------------------------------------------------
--�������ƣ�void MCP2515ModifyRegister(unsigned char address,unsigned char mask,unsigned char dat)
--�������ܣ��޸ļĴ�����ֵ
--���������unsigned char address����Ҫ�޸ĵļĴ����ĵ�ַ
			unsigned char mask	 �����룬Ϊ1��ʾ��λ�����޸�
			unsigned char dat	 ���޸�ֵ
--�����������
--˵	����λ�޸������ʽ��0x05  ��ַ�ֽ�	�����ֽ�  �����ֽ�
			���루���Σ���ȷ�������޸ļĴ����е���һλ��
						  1 ��ʾ����ԼĴ�������Ӧλ�����޸�
						  0 ���ֹ�޸�					   
			�����ֽ�	��ȷ���Ĵ���λ�޸ĵ����ս����
						  ��������ֽ���Ӧλ��Ϊ1�������ֽ��е�1��ʾ���Ĵ�����Ӧλ��1��
						  ��0���Ը�λ���㡣 
--------------------------------------------------------------------------------*/
void MCP2515ModifyRegister(unsigned char address,unsigned char mask,unsigned char dat)
{
	CS_High();
//	delay_us(10);					// CS��ֹʱ�䣬����50ns
	CS_Low();
	SPISendByte(MCP_BITMOD);		// λ�޸�����0x05,�����ַ�ֽڣ������ֽڣ������ֽ�
	SPISendByte(address);			// Ҫ�޸ļĴ����ĵ�ַ
	SPISendByte(mask);				// Ҫ���ε�λ
	SPISendByte(dat);	   
	CS_High();
}

/*
 *****************************************************
 * ����Զ����� ���úŵĲ����� ȥ���ò���
 *****************************************************
 */
unsigned char MCP2515Configrate(void)
{
	unsigned char set,cfg1,cfg2,cfg3;
	u8 CanFreqType;
	u16 ALLTQ;
	u8 PS2;

	set = 0;
	CanFreqType = MCP2515_CAN_FREQ_VALUE;
	switch(CanFreqType)
	{
		case _500K_:   
			ALLTQ = MCP2515_OSC / (2 *(MCP2515_BRP + 1) * 500000);	   											   
		break;
		case _250K_:
			ALLTQ = MCP2515_OSC / (2 *(MCP2515_BRP + 1) * 250000);	   											 
		break;
		case _125K_:
			ALLTQ = MCP2515_OSC / (2 *(MCP2515_BRP + 1) * 125000);	   											 
		break;
		case _100K_:
			ALLTQ = MCP2515_OSC / (2 *(MCP2515_BRP + 1) * 100000);	    											   
		break;
        case _50K_:
			ALLTQ = MCP2515_OSC / (2 *(MCP2515_BRP + 1) * 50000);	   									
		break;
		case _33_3K_:
			ALLTQ = MCP2515_OSC / (2 *(MCP2515_BRP + 1) * 33333);	   									
		break;
	}
	PS2 = ALLTQ - MCP2515_SYNC - MCP2515_PHSEG - MCP2515_PRSEG;
	cfg1 = 0x00 | MCP2515_BRP | (MCP2515_SJW << 6);
	cfg2 = 0x80|0x00|((MCP2515_PHSEG-1)<<3)|(MCP2515_PRSEG-1); 
	cfg3 = 0x00|0x00|(PS2-1);
	MCP2515SetRegister(MCP_CNF1_2A,cfg1);				//CNF1 ���üĴ���1 ��ַ0x2a
	MCP2515SetRegister(MCP_CNF2_29,cfg2);				//CNF2 ���üĴ���2 ��ַ0x29
	MCP2515SetRegister(MCP_CNF3_28,cfg3);				//CNF3 ���üĴ���3 ��ַ0x28

    Log("cnf: %x, %x, %x\r\n", cfg1, cfg2, cfg3);

	return set;
}


/*-------------------------------------------------------------------------------
--�������ƣ�void MCP2515SetCanFilter(unsigned char mcp_address,unsigned char ext,unsigned long can_id)
--�������ܣ�����mcp��������/�˲��Ĵ��������ͱ���id��
--���������unsigned char mcp_address��MCP��������/�˲��Ĵ�����ַ���ͻ�������ַ
			unsigned char ext		 ����չ��־λ			SID10 SID9 SID8 SID7 SID6 SID5 SID4 SID3
			unsigned long can_id	 ����Ӧ��id��			SID2 SID1 SID0 �� EXIDE �� EID17 EID16				EXIDE: Extended Identifier Enable bit
--�����������												EID15 EID14 EID13 EID12 EID11 EID10 EID9 EID8		1 = Filter is applied only to Extended Frames
															EID7 EID6 EID5 EID4 EID3 EID2 EID1 EID0 			0 = Filter is applied only to Standard Frames
--˵	����
--------------------------------------------------------------------------------*/
void MCP2515SetCanFilter(u8 mcp_address,u8 ext,u32 can_idValue)
{
	u32 canid;
	unsigned char tbufdat[4];

	#define	EXIDE		3
	
	canid = can_idValue;
	if(ext==0)												// ֻ���ձ�׼id������չID�޲���
	{
		canid=(unsigned int)(canid & 0x07ff);				// ��׼id 11λ��ֻȡ��11λ
		tbufdat[0]=(unsigned char)(canid/8);				// canid>>3,ȡID��8λ����tbufdat[0]
		tbufdat[1]=(unsigned char)((canid&0x07)<<5);		// ȡID��3λ����tbufdat[1]�ĸ�3λ
		tbufdat[2]=0;
		tbufdat[3]=0;
	}
	else
	{
		tbufdat[0] = (uint8_t) (canid>>21);
		tbufdat[1] = (((uint8_t) (canid>>16)) & 0x03) | (((uint8_t) (canid>>13)) & 0xE0) | (1<<EXIDE);
		tbufdat[2] = (uint8_t) (canid>>8);
		tbufdat[3] = (uint8_t) canid;
	}
	
	MCP2515SetRegisterS(mcp_address,tbufdat,4);
}

/*-------------------------------------------------------------------------------
--�������ƣ�void MCP2515InitCanBuffers(void)
--�������ܣ����㷢�Ϳ��ƼĴ��������ͻ�������������տ��ƼĴ�����
--�����������
--�����������
--˵	��������λΪ0ʱ�����еı��Ķ����գ�
				  Ϊ1ʱ������Ӧ��λ�����˲�����ʱҪ������λ�ˡ�
													 ����λΪ0��ֻ����0��������1
															 1��ֻ����1��������0
--------------------------------------------------------------------------------*/
void MCP2515InitCanBuffers(void)
{
	unsigned char i,a1,a2,a3;
	
	//��2�����μĴ���
	//RXMnSIDH-�������μĴ���n��׼��ʶ����λ��20h,24h��
	//RXMnSIDL-�������μĴ���n��׼��ʶ����λ��21h,25h��
	//RXMnEID8-�������μĴ���n��չ��ʶ����λ��22h,26h��
	//RXMnEID0-�������μĴ���n��չ��ʶ����λ��23h,27h��
	/*�˲����μĴ�����ȷ���˲����Ա�ʶ���е���Щλ����У�顣
	���μĴ�����Ҫȷ���Ա�ʶ���е���Щλ�����˲������ĳ����λ����Ϊ0��
	��Ӧ�ı�ʶ��λ�����Զ����ն��������˲�*/
	//����mcp�������μĴ���(ͼ���ұߵģ�ȫ��У�飬Ҫ������λ��)
	// RXB0 �����˲�����
	MCP2515SetCanFilter(MCP_RXM0SIDH_20,_MCP2515_M0_FAMT,_MCP2515_M0_ID);	// 0x20:RXM0SIDH-�������μĴ���0����׼ID��λ;0:��׼֡;0x7ff:��11λ(��3��8)����У�顣1=У�飬0=ֱ�ӽ���
	MCP2515SetCanFilter(MCP_RXF0SIDH_00,_MCP2515_ID1_FAMT,_MCP2515_ID1);	// 0x00:RXF0SIDH-�����˲��Ĵ���0��0��׼֡��1�����1��0�����0�����յ�ַΪ0x006�Ľڵ�����
	MCP2515SetCanFilter(MCP_RXF1SIDH_04,_MCP2515_ID2_FAMT,_MCP2515_ID2);	// 7ff ���˲���������ε� ȫ����
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//RXB1 �����˲����� ���ֻ���� ���� 4 �� ��ʶ��������  ������ ��Ϊ F  ��ֻ���� ӦҪ�� �ĸ����� ID ֵ
	MCP2515SetCanFilter(MCP_RXM1SIDH_24,_MCP2515_M1_FAMT,_MCP2515_M1_ID);	// �˲� / ���μĴ�����ֵ��
	MCP2515SetCanFilter(MCP_RXF2SIDH_08,_MCP2515_ID3_FAMT,_MCP2515_ID3);	// ����λ n   ����λ n    ���ı�ʶ��λ   ���ܻ�ܾ�λn
	MCP2515SetCanFilter(MCP_RXF3SIDH_10,_MCP2515_ID4_FAMT,_MCP2515_ID4);	//  0            x             x              ����
	MCP2515SetCanFilter(MCP_RXF4SIDH_14,_MCP2515_ID5_FAMT,_MCP2515_ID5);	//  1		     0             0           	  ����  
	MCP2515SetCanFilter(MCP_RXF5SIDH_18,_MCP2515_ID6_FAMT,_MCP2515_ID6);	//  1            0             1              �ܾ�
																			//	1		     1             0 			  �ܾ�	
																			// 	1            1             1              ����
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																		
	a1 = MCP_TXB0CTRL_30;													// TXB0CTRL���ͻ�����0���ƼĴ�����ַ
	a2 = MCP_TXB1CTRL_40;													// TXB1CTRL���ͻ�����1���ƼĴ�����ַ
	a3 = MCP_TXB2CTRL_50;													// TXB2CTRL���ͻ�����2���ƼĴ�����ַ 
	for(i=0;i<14;i++)														// ���ͻ�������14���ֽڣ�1�ֽڿ���+5�ֽ�װ�ر�׼����չ��ʶ�����ٲ�+8�ֽ�����
	{
		MCP2515SetRegister(a1,0);											// ���㷢�ͻ�����0��30-3d
		MCP2515SetRegister(a2,0);											// ���㷢�ͻ�����1��40-4d
		MCP2515SetRegister(a3,0);											// ���㷢�ͻ�����2��50-5d
		a1++;
		a2++;
		a3++;
	}
	//MCP2515SetRegister(MCP_RXB0CTRL_60,0);									// ������ջ�����0���ƼĴ���  ��������úܹؼ��������ǰ�ɶ�ָ�ʽ��������
#ifdef MCP2515_USE_FILTER
    MCP2515SetRegister(MCP_RXB1CTRL_70, 0);
#else
	MCP2515SetRegister(MCP_RXB1CTRL_70, 1<<5 | 1<<6);									// ������ջ�����1���ƼĴ���  ��������úܹؼ��������ǰ�ɶ�ָ�ʽ�������� ����
#endif
}


/*-------------------------------------------------------------------------------
--�������ƣ�unsigned char MCP2515SetCanCtrlMode(unsigned char newmode)
--�������ܣ�����MCP2515�Ĺ���ģʽ
--���������unsigned char newmode��MCP2515������ģʽ֮һ
 				MODE_NORMAL_00	   	0x00
 				MODE_SLEEP_20		0x20
 				MODE_LOOPBACK_40	0x40
 				MODE_LISTENONLY_60	0x60
 				MODE_CONFIG_80	   	0x80
--������������� ʵ�ʵĹ���ģʽ������֮����Ҫ�жϷ���ֵ��ȷ���Ƿ����óɹ�
--˵	���� 
--------------------------------------------------------------------------------*/
unsigned char MCP2515SetCanCtrlMode(unsigned char newmode)  
{
	unsigned char i;
	
	MCP2515ModifyRegister(MCP_CANCTRL_0F,0xe0,newmode); 	// 0x0f=CANCTRL(CAN���ƼĴ�����ַ)
															// 0xe0=���루���ԶԼĴ����ĸ���λ�����޸ģ�
	i=MCP2515ReadRegister(MCP_CANCTRL_0F);					// ��ȡ��ַΪ0x0f��CANCTRL���Ĵ�����ֵ  (����Ӧ�ò��ԣ��Ƕ� ״̬ MCP_CANSTAT )  Ҳ���������ɣ����趨��ֵ�����������Ƿ����óɹ�
	i&=0xe0;												// CANCTRL�Ĵ�����ֵ���������룬������ȡ���õ�ģʽ
	if(i==newmode)
		return 0;											// ���ù���ģʽ�ɹ�����0
	else
		return 1;											// ���ù���ģʽʧ�ܷ���1
}

/*-------------------------------------------------------------------------------
--�������ƣ�unsigned char InitCan(void)
--�������ܣ�CAN��ʼ��
--�����������
--�����������ʼ���Ƿ�ɹ� 0�ɹ� 1ʧ��
--˵	����������ģʽ������ͨ�ŵ����ʣ����㷢�ͻ�����������ͨ�ŷ�ʽ���жϣ���������Ϊ����ģʽ��
--------------------------------------------------------------------------------*/
u8 InitCan_MCP2525data(void)
{
	u8 res;
//	CS = 0;									// ѡ��Ƭѡ
//	RET = 1;								// ��λ
//	RET = 0;								// ʹ��
//	delay_ms(10);							// �͵�ƽ���ٱ���2us
//	RET = 1;
//	CS = 1;
	//���ò���
	res=MCP2515SetCanCtrlMode(MODE_CONFIG_80);	  	// ����ģʽ
	if(res==1)return res;							// ����ʧ�ܣ�ֱ���˳�
	//����ͨ������
	MCP2515Configrate();							// ������ 
	//���㷢�Ϳ��ƼĴ��������ͻ�������������տ��ƼĴ�����
	MCP2515InitCanBuffers();
	//�����ж�
	MCP2515SetRegister(MCP_CANINTF,0x00);			// ��������жϱ�־λ  ���� ����Ҫ��Ҫ����			
	MCP2515SetRegister(MCP_CANINTE_2B, 0x42);		// ���￪����RX1�ж�,�����ж�
    
	//����Ϊ��������ģʽ
	res = MCP2515SetCanCtrlMode(MODE_NORMAL_00);
    
	return 0;
}


//ͨ��Ӳ����λ
CL_INLINE void MCP2515_Reset(void)
{
    GPIO_ResetBits(MCP2515_RST_PORT_GROUP, MCP2515_RST_PORT_NUM);
    DelayMs(2);
    GPIO_SetBits(MCP2515_RST_PORT_GROUP, MCP2515_RST_PORT_NUM);
    DelayMs(10);
//    SPISendByte(MCP_RESET);
}


void cInitMCP2515(void)
{
    Log("mcp2515 init\r\n");

	CS_High(); 							

    MCP2515_Reset();    //��SPI���λ
    
	ucMCP2515RevMark = 0;
	InitCan_MCP2525data();				//
	DelayMs(10);
}



/*
 *********************************************************
 * SPI MCP2515 �ڵ�����
 *********************************************************
 */
void MCP2515_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
	//CS
	GPIO_InitStructure.GPIO_Pin = MCP2515_CS_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_CS_PORT_GROUP, &GPIO_InitStructure);

    //CLK
	GPIO_InitStructure.GPIO_Pin = MCP2515_CLK_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_CLK_PORT_GROUP, &GPIO_InitStructure);

    //MISO
	GPIO_InitStructure.GPIO_Pin = MCP2515_MISO_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_MISO_PORT_GROUP, &GPIO_InitStructure);

    //MOSI
	GPIO_InitStructure.GPIO_Pin = MCP2515_MOSI_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_MOSI_PORT_GROUP, &GPIO_InitStructure);

    //INT
    GPIO_InitStructure.GPIO_Pin = MCP2515_INT_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_INT_PORT_GROUP, &GPIO_InitStructure);

    //RST
    GPIO_InitStructure.GPIO_Pin = MCP2515_RST_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_RST_PORT_GROUP, &GPIO_InitStructure);
    GPIO_SetBits(MCP2515_RST_PORT_GROUP, MCP2515_RST_PORT_NUM);

    
//	CS = 1; SCK = 1; SI = 1; RET = 1;
    CS_High();
    CLK_High();
    MOSI_High();
    

    GPIO_EXTILineConfig(MCP2515_INT_PORT_SOURCE, MCP2515_INT_PIN_SOURCE);    // ��EXIT�����ӵ�IO��
	EXTI_InitStructure.EXTI_Line = MCP2515_INT_LINE;						// ��·ѡ��
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				// �¼�ѡ��
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			// ����ģʽ
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						// �����ж�
	EXTI_Init(&EXTI_InitStructure);	

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
	cInitMCP2515();

    
}


void MCP2515_Exit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    
	//CS
	GPIO_InitStructure.GPIO_Pin = MCP2515_CS_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_CS_PORT_GROUP, &GPIO_InitStructure);

    //CLK
	GPIO_InitStructure.GPIO_Pin = MCP2515_CLK_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_CLK_PORT_GROUP, &GPIO_InitStructure);

    //MOSI
	GPIO_InitStructure.GPIO_Pin = MCP2515_MOSI_PORT_NUM;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCP2515_MOSI_PORT_GROUP, &GPIO_InitStructure);

    GPIO_EXTILineConfig(MCP2515_INT_PORT_SOURCE, MCP2515_INT_PIN_SOURCE);    // ��EXIT�����ӵ�IO��
	EXTI_InitStructure.EXTI_Line = MCP2515_INT_LINE;						// ��·ѡ��
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				// �¼�ѡ��
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			// ����ģʽ
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;						// �����ж�
	EXTI_Init(&EXTI_InitStructure);	

}

/*
RXBnSIDH �C RECEIVE BUFFER n STANDARD IDENTIFIER HIGH
(ADDRESS: 61h, 71h)
		SID10 SID9 SID8 SID7 SID6 SID5 SID4 SID3
RXBnSIDL �C RECEIVE BUFFER n STANDARD IDENTIFIER LOW
(ADDRESS: 62h, 72h)
		SID2 SID1 SID0 SRR IDE �� EID17 EID16

SRR: Standard Frame Remote Transmit Request bit (valid only if IDE bit = ��0��)
	1 = Standard Frame Remote Transmit Request Received
	0 = Standard Data Frame Received

IDE: Extended Identifier Flag bit
	This bit indicates whether the received message was a Standard or an Extended Frame
	1 = Received message was an Extended Frame
	0 = Received message was a Standard Frame
	
RXBnEID8 �C RECEIVE BUFFER n EXTENDED IDENTIFIER HIGH
(ADDRESS: 63h, 73h)
	EID15 EID14 EID13 EID12 EID11 EID10 EID9 EID8
	
RXBnEID0 �C RECEIVE BUFFER n EXTENDED IDENTIFIER LOW
(ADDRESS: 64h, 74h)
	EID7 EID6 EID5 EID4 EID3 EID2 EID1 EID0
	
 */
/*
 *****************************************************************
 * DEMO application
 *
 *	if(CAN_MCP2515MessagePending()									// ��ѯ��ʽ���ж�����Ч��CAN��Ϣ FIFO0
 *	{
 *		CAN_Receive_MCP2515();										// ��FIFO������ϢCAN�����ں����жϺʹ���
 *		switch(CANRxSPIMassage.Id_Add)		
 *		{
 *			;
 *		}
 *	}
 * ģ�� STM32 �Ĳ�ѯ�ṹ
 *****************************************************************
 */
u8 CAN_MCP2515MessagePending(void)									// ��ѯ��ʽ���ж�����Ч��CAN��Ϣ FIFO0
{
	return ucMCP2515RevMark;
}

uint8_t CAN_Receive_MCP2515(void)
{
	unsigned char tbufdat[4];
	unsigned char i,len,Sta_Int;
	u32 add = 0;
    uint8_t res = 0;

    MCP2515ModifyRegister(MCP_EFLG, MCP_RX1_OVR, 0);
    
// ��ȡ�жϱ�־�Ĵ���
	Sta_Int = MCP2515ReadRegister(MCP_CANINTF);
    if(Sta_Int & MCP_WAKIF)
    {
//        MCP2515ModifyRegister(MCP_CANINTF, MCP_WAKIF, 0x00);  //�廽���ж�
//        MCP2515SetCanCtrlMode(MODE_NORMAL_00);
        cInitMCP2515();

        res = 1;
    }


	if(Sta_Int&MCP_RX1IF)								//���Ϊ���ջ�����0�ж�    MCP2515ModifyRegister(MCP_CANINTF,0x02,0x00);		   // ������ջ���0�жϱ�־λ
	{
		for(i = 0;i < 4;i++)
		{
			tbufdat[i] = MCP2515ReadRegister(MCP_RXB1SIDH_71 + i);
		}
		if(tbufdat[1] & 0x08)							// ��չ
		{	// SID10 SID9 SID8 SID7 SID6 SID5 SID4 SID3 | SID2 SID1 SID0 SRR IDE �� EID17 EID16 | EID15 EID14 EID13 EID12 EID11 EID10 EID9 EID8 | EID7 EID6 EID5 EID4 EID3 EID2 EID1 EID0
			add = (u32)tbufdat[3] | (u32)tbufdat[2] << 8 | (u32)(tbufdat[1] & 0x03) << 16 | (u32)((tbufdat[1] & 0xE0))<<13 | (u32)(tbufdat[0] << 21);
			CANRxSPIMassage.Id_Add = add;
			CANRxSPIMassage.Ext = _MCP2515_CAN_EXT;
		}
		else											// ��׼
		{
			if(tbufdat[1] & 0x10)	CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_REMOTE;
			else					CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_DATA;
			add = ((u32)(tbufdat[0]))<< 3;	 
			add = add | (((u32)(tbufdat[1]))>> 5);
			CANRxSPIMassage.Id_Add = add;
			CANRxSPIMassage.Ext = _MCP2515_CAN_STD;
            
		}
		len = MCP2515ReadRegister(MCP_RXB1CTRL_70+5);		 //��ȡ���ݳ���
		if(_MCP2515_CAN_EXT  == CANRxSPIMassage.Ext)
		{
			if(len & 0x40)	CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_REMOTE;
			else			CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_DATA;
		}
		len = len & 0x0f;
        CANRxSPIMassage.DLC = len;
		for(i=0;i<len;i++)								//��ӡ���յ�������
		{	 
			CANRxSPIMassage.Data[i] = MCP2515ReadRegister(MCP_RXB1CTRL_70+6+i);
		}

        MCP2515ModifyRegister(MCP_CANINTF, MCP_RX1IF, 0x00);				//��������ж�	
        res = 2;
	}
	return res;
}

/*
 *************************************************
 * �����ݴ� TX0 ���ͳ�ȥ
 *	CANTxMessage = CANTxMessage1;
 *	CAN_Transmit(&CANTxMessage);
 *************************************************
 */

void CAN_MCP2515_Transmit(CAN_SPIMessage * Message)
{
	u8 i,len;
    MCP2515ModifyRegister(MCP_TXB0CTRL_30, 1 << 3, 0);
    
	MCP2515SetCanFilter(MCP_TXB0SIDH_31,Message->Ext,Message->Id_Add);	   // 31 32 33 34 д ID ��
	len = Message->DLC;
	if(len > 8)   len = 8;
	i = len;
	if(Message->Remote) i = i | 0x40;
	MCP2515SetRegister(0x35,i); 											// ��дҪ���͵��ֽ���
	for(i = 0;i < len;i++)
	{
		MCP2515SetRegister(0x36 + i,Message->Data[i]);
	}
	MCP2515ModifyRegister(MCP_TXB0CTRL_30,0x0b,0x0b);					   // ���Ϳ��ƼĴ���
}

/*
 *****************************************************************
 * ������� MCP2515 ���� ���� ģʽ
 * ���Դ��ⲿ(mcu)���� Ҳ�����Զ����߻���
 *****************************************************************
 */
void cSetMCP2515ToSleepMode(void)
{
	MCP2515SetCanCtrlMode(MODE_SLEEP_20);									// ��������ģʽ
}

/*
 ******************************************************************
 * �ⲿ�������� MCP2515
 * ������Բ��ã���Ϊ�˳�����ģʽʱ ���е� IC ȫ����ʼ����һ��
 ******************************************************************
 */
void cFWWakeMCP2515(void)
{
    MCP2515ModifyRegister(MCP_CANINTF,MCP_WAKIF, 0xff);									// 	
}

/*
 ******************************************************************
 * ������,����������߹ر�
 ******************************************************************
 */
void MCP2515_ErrorCheck(void)
{
    uint8_t data;
    data = MCP2515ReadRegister(MCP_EFLG);
    
    if(data & MCP_ERR_FLAG) //������
    {
        cInitMCP2515();
    }
}


