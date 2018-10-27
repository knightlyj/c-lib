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

//外部中断 10-15 服务程序
void EXTI15_10_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line12) != RESET)						//判断中断发生来源
	{
	    EXTI_ClearITPendingBit(EXTI_Line12); 							//清除 LINE0 上的中断标志位
		ucMCP2515RevMark = 1;										// 设置标志
    	
	}	 
	
//    fAlarmRTCInt = FALSE;
}



/*-------------------------------------------------------------------------------
--函数名称：void SPISendByte(unsigned char dat)
--函数功能：软件模拟SPI时序，通过SPI写一个字节数据
--输入参数：unsigned char dat：要通过SPI发送的数据
--输出参数：无
--说	明： CS	--\_______________________________________________/--------\___
			 SCK00 ___/---\___/---\____/---\___/---\___/---\___/----\____
			 SI   --\_MSB_/-------\________/-------\___LSB_/--------
			 SO   ----------高阻态----------------------------------
			 SCK在00模式下，SI在SCK的上升沿写入，高位在前。
			 以下在时钟频率小于10M时参考（本模块4M）
			 CS建立时间(CS拉低到SCK的上升沿)最小：50ns
			 CS保持时间(CS拉高到拉高前SCK的上升沿)最小：50ns
			 CS禁止时间(CS拉高到下一次拉低)最小:50ns
			 数据建立和数据保持时间都是10ns
			 CLK上升和下降沿最大2us

			 函数发送完后，CS=0,还可以接着发送数据，如下所示
			 SPISendByte（）
			 SPISendByte（）
			 SPISendByte（）
			 CS=1;//最后再拉高CS
--------------------------------------------------------------------------------*/
void SPISendByte(unsigned char dat)
{
	unsigned char i;
	CS_Low();							// CS低电平有效，可在外部可在写多个字节数据
//	delay_us(50);					// CS建立时间，最小50ns，这里是50us
	for(i=0;i<8;i++)
	{
		CLK_Low();					// 低电平时允许总线数据变化(为上升沿做准备)，上升沿MCP2515接收数据
//		delay_us(5);		 
		if(dat&0x80)				// 使用0x80，是因为高位先发送
		    MOSI_High(); 				// 向总线输出数据
		else
			MOSI_Low();
//		delay_us(5);				// 数据建立时间，大于10ns即可，这里是5us
		CLK_High();
		dat=dat<<1;
//		delay_us(5);				// 数据保持时间，大于10ns即可，这里是5us
	}
}

/*-------------------------------------------------------------------------------
--函数名称：unsigned char SPIRecvByte(void)
--函数功能：软件模拟SPI时序，通过SPI接收一个字节数据
--输入参数：无
--输出参数：返回一个字节的接收数据
--说	明： CS	--\_______________________________________________/--------\___
			 SCK-----\___/---\____/---\___/---\___/---\___/----\____
			 SO   -----\___MSB__/-------\__
			 SI   ----------任意状态-------
			 SCK的下降沿接收数据
--------------------------------------------------------------------------------*/
unsigned char SPIRecvByte(void)
{
	unsigned char dat=0;				// 接收的数据
	unsigned char i;
	CS_Low();								// 整个过程中CS为低电平
	for(i=0;i<8;i++)
	{
		dat=dat<<1; 					// 左移，SO先接收到的是高位
		CLK_High();
//		delay_us(5);					// SCK高电平维持时间大于45ns,这里是5us
		CLK_Low();						// 下降沿
//		delay_us(5);					// 维持数据的操作
		if(Read_MISO())	dat=dat|0x01;			// 最后一位置1
		else	dat=dat&0xfe;			// 最后一位清0
	}
	CLK_Low();							// SCK最后拉低
	return dat;
}
/*-------------------------------------------------------------------------------
--函数名称：void MCP2515SetRegister(unsigned char address,unsigned char value)
--函数功能：设置MCP2515寄存器，写一字节数据
--输入参数：unsigned char address：需要设置的寄存器的地址
			unsigned char value  ：需要设置的具体值
--输出参数：无
--说	明： 
--------------------------------------------------------------------------------*/
void MCP2515SetRegister(unsigned char address,unsigned char value)
{
	CS_High(); 								// 在调用其他命令时，CS要先拉高
//	delay_us(5);							// CS禁止时间最小50ns,这里是5us
	SPISendByte(MCP_WRITE); 				// 字节写命令0x02,后跟一字节地址，一字节数据
	SPISendByte(address);
	SPISendByte(value); 					// 连续SPI写
	CS_High();
//	delay_us(5);							// CS禁止时间最小50ns,这里是5us
}

/*-------------------------------------------------------------------------------
--函数名称：void MCP2515SetRegisterS(unsigned char address,unsigned char values[],
									 unsigned char n)
--函数功能：设置MCP2515发送缓冲区，写多个寄存器
--输入参数：unsigned char address：需要设置的寄存器的地址
			unsigned char values[]：需要设置的缓冲区的头指针
			unsigned char n
--输出参数：无
--说	明： 
--------------------------------------------------------------------------------*/
void MCP2515SetRegisterS(unsigned char address,unsigned char values[],unsigned char n)
{
	unsigned char i;
	CS_High();									// 操作之前与之后，CS要拉高
//	delay_us(5);							// CS禁止时间最小50ns,这里是5us
	SPISendByte(MCP_WRITE); 				// 发送写命令0x02,后跟地址字节，数据字节
	SPISendByte(address);					// 目的起始地址
	for(i=0;i<n;i++)
		SPISendByte(values[i]);
	CS_High();
//	delay_us(5);							// CS禁止时间最小50ns,这里是5us
}

/*-------------------------------------------------------------------------------
--函数名称：unsigned char MCP2515ReadRegister(unsigned char address)
--函数功能：读取指定寄存器的值
--输入参数：unsigned char address：需要读取的寄存器的地址
--输出参数：返回值为读取的结果
--说	明： 
--------------------------------------------------------------------------------*/
unsigned char MCP2515ReadRegister(unsigned char address)
{
	unsigned char ret;
	CS_High();
//	delay_us(5);							// 起始与结尾CS置1，器件不选中，CS禁止时间最小50ns,这里是5us
	SPISendByte(MCP_READ);					// 读命令：0x03,后跟地址字节，接着在SI上返回数据
	SPISendByte(address);					// 寄存器地址
	ret=SPIRecvByte();
	CS_High();
	return ret;
}

/*-------------------------------------------------------------------------------
--函数名称：void MCP2515ModifyRegister(unsigned char address,unsigned char mask,unsigned char dat)
--函数功能：修改寄存器的值
--输入参数：unsigned char address：需要修改的寄存器的地址
			unsigned char mask	 ：掩码，为1表示该位可以修改
			unsigned char dat	 ：修改值
--输出参数：无
--说	明：位修改命令格式：0x05  地址字节	掩码字节  数据字节
			掩码（屏蔽）：确定允许修改寄存器中的哪一位。
						  1 表示允许对寄存器中相应位进行修改
						  0 则禁止修改					   
			数据字节	：确定寄存器位修改的最终结果。
						  如果屏蔽字节相应位置为1，数据字节中的1表示将寄存器对应位置1，
						  而0将对该位清零。 
--------------------------------------------------------------------------------*/
void MCP2515ModifyRegister(unsigned char address,unsigned char mask,unsigned char dat)
{
	CS_High();
//	delay_us(10);					// CS禁止时间，大于50ns
	CS_Low();
	SPISendByte(MCP_BITMOD);		// 位修改命令0x05,后跟地址字节，屏蔽字节，数据字节
	SPISendByte(address);			// 要修改寄存器的地址
	SPISendByte(mask);				// 要屏蔽的位
	SPISendByte(dat);	   
	CS_High();
}

/*
 *****************************************************
 * 软件自动按照 设置号的波特率 去配置参数
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
	MCP2515SetRegister(MCP_CNF1_2A,cfg1);				//CNF1 配置寄存器1 地址0x2a
	MCP2515SetRegister(MCP_CNF2_29,cfg2);				//CNF2 配置寄存器2 地址0x29
	MCP2515SetRegister(MCP_CNF3_28,cfg3);				//CNF3 配置寄存器3 地址0x28

    Log("cnf: %x, %x, %x\r\n", cfg1, cfg2, cfg3);

	return set;
}


/*-------------------------------------------------------------------------------
--函数名称：void MCP2515SetCanFilter(unsigned char mcp_address,unsigned char ext,unsigned long can_id)
--函数功能：设置mcp验收屏蔽/滤波寄存器，或发送报文id。
--输入参数：unsigned char mcp_address：MCP接收屏蔽/滤波寄存器地址或发送缓冲区地址
			unsigned char ext		 ：扩展标志位			SID10 SID9 SID8 SID7 SID6 SID5 SID4 SID3
			unsigned long can_id	 ：相应的id码			SID2 SID1 SID0 — EXIDE — EID17 EID16				EXIDE: Extended Identifier Enable bit
--输出参数：无												EID15 EID14 EID13 EID12 EID11 EID10 EID9 EID8		1 = Filter is applied only to Extended Frames
															EID7 EID6 EID5 EID4 EID3 EID2 EID1 EID0 			0 = Filter is applied only to Standard Frames
--说	明：
--------------------------------------------------------------------------------*/
void MCP2515SetCanFilter(u8 mcp_address,u8 ext,u32 can_idValue)
{
	u32 canid;
	unsigned char tbufdat[4];

	#define	EXIDE		3
	
	canid = can_idValue;
	if(ext==0)												// 只接收标准id，对扩展ID无操作
	{
		canid=(unsigned int)(canid & 0x07ff);				// 标准id 11位，只取低11位
		tbufdat[0]=(unsigned char)(canid/8);				// canid>>3,取ID高8位放入tbufdat[0]
		tbufdat[1]=(unsigned char)((canid&0x07)<<5);		// 取ID低3位放入tbufdat[1]的高3位
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
--函数名称：void MCP2515InitCanBuffers(void)
--函数功能：清零发送控制寄存器，发送缓冲区；清零接收控制寄存器等
--输入参数：无
--输出参数：无
--说	明：屏蔽位为0时，所有的报文都接收；
				  为1时，对相应的位进行滤波，这时要看过滤位了。
													 过滤位为0，只接收0；不接收1
															 1，只接收1；不接收0
--------------------------------------------------------------------------------*/
void MCP2515InitCanBuffers(void)
{
	unsigned char i,a1,a2,a3;
	
	//有2个屏蔽寄存器
	//RXMnSIDH-验收屏蔽寄存器n标准标识符高位（20h,24h）
	//RXMnSIDL-验收屏蔽寄存器n标准标识符低位（21h,25h）
	//RXMnEID8-验收屏蔽寄存器n扩展标识符高位（22h,26h）
	//RXMnEID0-验收屏蔽寄存器n扩展标识符低位（23h,27h）
	/*滤波屏蔽寄存器：确定滤波器对标识符中的哪些位进行校验。
	屏蔽寄存器主要确定对标识符中的哪些位进行滤波。如果某屏蔽位设置为0，
	对应的标识符位将被自动接收而不进行滤波*/
	//设置mcp验收屏蔽寄存器(图中右边的，全部校验，要看过滤位了)
	// RXB0 接收滤波设置
	MCP2515SetCanFilter(MCP_RXM0SIDH_20,_MCP2515_M0_FAMT,_MCP2515_M0_ID);	// 0x20:RXM0SIDH-验收屏蔽寄存器0，标准ID高位;0:标准帧;0x7ff:对11位(高3低8)进行校验。1=校验，0=直接接收
	MCP2515SetCanFilter(MCP_RXF0SIDH_00,_MCP2515_ID1_FAMT,_MCP2515_ID1);	// 0x00:RXF0SIDH-验收滤波寄存器0；0标准帧；1想接收1，0想接收0；接收地址为0x006的节点数据
	MCP2515SetCanFilter(MCP_RXF1SIDH_04,_MCP2515_ID2_FAMT,_MCP2515_ID2);	// 7ff 不滤波，配合屏蔽的 全接受
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//RXB1 接收滤波设置 软件只允许 配置 4 个 标识符过滤器  将屏蔽 配为 F  ，只接收 应要的 四个过滤 ID 值
	MCP2515SetCanFilter(MCP_RXM1SIDH_24,_MCP2515_M1_FAMT,_MCP2515_M1_ID);	// 滤波 / 屏蔽寄存器真值表
	MCP2515SetCanFilter(MCP_RXF2SIDH_08,_MCP2515_ID3_FAMT,_MCP2515_ID3);	// 屏蔽位 n   过滤位 n    报文标识符位   接受或拒绝位n
	MCP2515SetCanFilter(MCP_RXF3SIDH_10,_MCP2515_ID4_FAMT,_MCP2515_ID4);	//  0            x             x              接受
	MCP2515SetCanFilter(MCP_RXF4SIDH_14,_MCP2515_ID5_FAMT,_MCP2515_ID5);	//  1		     0             0           	  接受  
	MCP2515SetCanFilter(MCP_RXF5SIDH_18,_MCP2515_ID6_FAMT,_MCP2515_ID6);	//  1            0             1              拒绝
																			//	1		     1             0 			  拒绝	
																			// 	1            1             1              接受
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																		
	a1 = MCP_TXB0CTRL_30;													// TXB0CTRL发送缓冲区0控制寄存器地址
	a2 = MCP_TXB1CTRL_40;													// TXB1CTRL发送缓冲区1控制寄存器地址
	a3 = MCP_TXB2CTRL_50;													// TXB2CTRL发送缓冲区2控制寄存器地址 
	for(i=0;i<14;i++)														// 发送缓冲区是14个字节，1字节控制+5字节装载标准和扩展标识符及仲裁+8字节数据
	{
		MCP2515SetRegister(a1,0);											// 清零发送缓冲区0：30-3d
		MCP2515SetRegister(a2,0);											// 清零发送缓冲区1：40-4d
		MCP2515SetRegister(a3,0);											// 清零发送缓冲区2：50-5d
		a1++;
		a2++;
		a3++;
	}
	//MCP2515SetRegister(MCP_RXB0CTRL_60,0);									// 清零接收缓冲区0控制寄存器  这里的设置很关键，决定是按啥种格式接收数据
#ifdef MCP2515_USE_FILTER
    MCP2515SetRegister(MCP_RXB1CTRL_70, 0);
#else
	MCP2515SetRegister(MCP_RXB1CTRL_70, 1<<5 | 1<<6);									// 清零接收缓冲区1控制寄存器  这里的设置很关键，决定是按啥种格式接收数据 ？？
#endif
}


/*-------------------------------------------------------------------------------
--函数名称：unsigned char MCP2515SetCanCtrlMode(unsigned char newmode)
--函数功能：设置MCP2515的工作模式
--输入参数：unsigned char newmode：MCP2515的五种模式之一
 				MODE_NORMAL_00	   	0x00
 				MODE_SLEEP_20		0x20
 				MODE_LOOPBACK_40	0x40
 				MODE_LISTENONLY_60	0x60
 				MODE_CONFIG_80	   	0x80
--输出参数：返回 实际的工作模式，返回之后需要判断返回值来确定是否设置成功
--说	明： 
--------------------------------------------------------------------------------*/
unsigned char MCP2515SetCanCtrlMode(unsigned char newmode)  
{
	unsigned char i;
	
	MCP2515ModifyRegister(MCP_CANCTRL_0F,0xe0,newmode); 	// 0x0f=CANCTRL(CAN控制寄存器地址)
															// 0xe0=掩码（可以对寄存器的高三位进行修改）
	i=MCP2515ReadRegister(MCP_CANCTRL_0F);					// 读取地址为0x0f（CANCTRL）寄存器的值  (这里应该不对，是读 状态 MCP_CANSTAT )  也可以这样吧，将设定的值读回来，看是否设置成功
	i&=0xe0;												// CANCTRL寄存器的值与掩码相与，用来提取设置的模式
	if(i==newmode)
		return 0;											// 设置工作模式成功返回0
	else
		return 1;											// 设置工作模式失败返回1
}

/*-------------------------------------------------------------------------------
--函数名称：unsigned char InitCan(void)
--函数功能：CAN初始化
--输入参数：无
--输出参数：初始化是否成功 0成功 1失败
--说	明：在配置模式下设置通信的速率，清零发送缓冲区，数据通信方式（中断）；再设置为正常模式。
--------------------------------------------------------------------------------*/
u8 InitCan_MCP2525data(void)
{
	u8 res;
//	CS = 0;									// 选中片选
//	RET = 1;								// 复位
//	RET = 0;								// 使能
//	delay_ms(10);							// 低电平至少保持2us
//	RET = 1;
//	CS = 1;
	//设置参数
	res=MCP2515SetCanCtrlMode(MODE_CONFIG_80);	  	// 配置模式
	if(res==1)return res;							// 配置失败，直接退出
	//设置通信速率
	MCP2515Configrate();							// 波特率 
	//清零发送控制寄存器，发送缓冲区；清零接收控制寄存器等
	MCP2515InitCanBuffers();
	//采用中断
	MCP2515SetRegister(MCP_CANINTF,0x00);			// 清除所有中断标志位  ？？ 这里要不要？？			
	MCP2515SetRegister(MCP_CANINTE_2B, 0x42);		// 这里开启了RX1中断,唤醒中断
    
	//设置为正常工作模式
	res = MCP2515SetCanCtrlMode(MODE_NORMAL_00);
    
	return 0;
}


//通过硬件复位
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

    MCP2515_Reset();    //用SPI命令复位
    
	ucMCP2515RevMark = 0;
	InitCan_MCP2525data();				//
	DelayMs(10);
}



/*
 *********************************************************
 * SPI MCP2515 口的配置
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
    

    GPIO_EXTILineConfig(MCP2515_INT_PORT_SOURCE, MCP2515_INT_PIN_SOURCE);    // 将EXIT线连接到IO口
	EXTI_InitStructure.EXTI_Line = MCP2515_INT_LINE;						// 线路选择
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				// 事件选择
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			// 触发模式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						// 启动中断
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

    GPIO_EXTILineConfig(MCP2515_INT_PORT_SOURCE, MCP2515_INT_PIN_SOURCE);    // 将EXIT线连接到IO口
	EXTI_InitStructure.EXTI_Line = MCP2515_INT_LINE;						// 线路选择
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				// 事件选择
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			// 触发模式
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;						// 启动中断
	EXTI_Init(&EXTI_InitStructure);	

}

/*
RXBnSIDH – RECEIVE BUFFER n STANDARD IDENTIFIER HIGH
(ADDRESS: 61h, 71h)
		SID10 SID9 SID8 SID7 SID6 SID5 SID4 SID3
RXBnSIDL – RECEIVE BUFFER n STANDARD IDENTIFIER LOW
(ADDRESS: 62h, 72h)
		SID2 SID1 SID0 SRR IDE — EID17 EID16

SRR: Standard Frame Remote Transmit Request bit (valid only if IDE bit = ‘0’)
	1 = Standard Frame Remote Transmit Request Received
	0 = Standard Data Frame Received

IDE: Extended Identifier Flag bit
	This bit indicates whether the received message was a Standard or an Extended Frame
	1 = Received message was an Extended Frame
	0 = Received message was a Standard Frame
	
RXBnEID8 – RECEIVE BUFFER n EXTENDED IDENTIFIER HIGH
(ADDRESS: 63h, 73h)
	EID15 EID14 EID13 EID12 EID11 EID10 EID9 EID8
	
RXBnEID0 – RECEIVE BUFFER n EXTENDED IDENTIFIER LOW
(ADDRESS: 64h, 74h)
	EID7 EID6 EID5 EID4 EID3 EID2 EID1 EID0
	
 */
/*
 *****************************************************************
 * DEMO application
 *
 *	if(CAN_MCP2515MessagePending()									// 查询方式。判断是有效的CAN信息 FIFO0
 *	{
 *		CAN_Receive_MCP2515();										// 从FIFO接收信息CAN，用于后续判断和处理
 *		switch(CANRxSPIMassage.Id_Add)		
 *		{
 *			;
 *		}
 *	}
 * 模拟 STM32 的查询结构
 *****************************************************************
 */
u8 CAN_MCP2515MessagePending(void)									// 查询方式。判断是有效的CAN信息 FIFO0
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
    
// 获取中断标志寄存器
	Sta_Int = MCP2515ReadRegister(MCP_CANINTF);
    if(Sta_Int & MCP_WAKIF)
    {
//        MCP2515ModifyRegister(MCP_CANINTF, MCP_WAKIF, 0x00);  //清唤醒中断
//        MCP2515SetCanCtrlMode(MODE_NORMAL_00);
        cInitMCP2515();

        res = 1;
    }


	if(Sta_Int&MCP_RX1IF)								//如果为接收缓冲器0中断    MCP2515ModifyRegister(MCP_CANINTF,0x02,0x00);		   // 清零接收缓冲0中断标志位
	{
		for(i = 0;i < 4;i++)
		{
			tbufdat[i] = MCP2515ReadRegister(MCP_RXB1SIDH_71 + i);
		}
		if(tbufdat[1] & 0x08)							// 扩展
		{	// SID10 SID9 SID8 SID7 SID6 SID5 SID4 SID3 | SID2 SID1 SID0 SRR IDE — EID17 EID16 | EID15 EID14 EID13 EID12 EID11 EID10 EID9 EID8 | EID7 EID6 EID5 EID4 EID3 EID2 EID1 EID0
			add = (u32)tbufdat[3] | (u32)tbufdat[2] << 8 | (u32)(tbufdat[1] & 0x03) << 16 | (u32)((tbufdat[1] & 0xE0))<<13 | (u32)(tbufdat[0] << 21);
			CANRxSPIMassage.Id_Add = add;
			CANRxSPIMassage.Ext = _MCP2515_CAN_EXT;
		}
		else											// 标准
		{
			if(tbufdat[1] & 0x10)	CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_REMOTE;
			else					CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_DATA;
			add = ((u32)(tbufdat[0]))<< 3;	 
			add = add | (((u32)(tbufdat[1]))>> 5);
			CANRxSPIMassage.Id_Add = add;
			CANRxSPIMassage.Ext = _MCP2515_CAN_STD;
            
		}
		len = MCP2515ReadRegister(MCP_RXB1CTRL_70+5);		 //读取数据长度
		if(_MCP2515_CAN_EXT  == CANRxSPIMassage.Ext)
		{
			if(len & 0x40)	CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_REMOTE;
			else			CANRxSPIMassage.Remote = _MCP2515_CAN_RTR_DATA;
		}
		len = len & 0x0f;
        CANRxSPIMassage.DLC = len;
		for(i=0;i<len;i++)								//打印接收到的数据
		{	 
			CANRxSPIMassage.Data[i] = MCP2515ReadRegister(MCP_RXB1CTRL_70+6+i);
		}

        MCP2515ModifyRegister(MCP_CANINTF, MCP_RX1IF, 0x00);				//清除接收中断	
        res = 2;
	}
	return res;
}

/*
 *************************************************
 * 将数据从 TX0 发送出去
 *	CANTxMessage = CANTxMessage1;
 *	CAN_Transmit(&CANTxMessage);
 *************************************************
 */

void CAN_MCP2515_Transmit(CAN_SPIMessage * Message)
{
	u8 i,len;
    MCP2515ModifyRegister(MCP_TXB0CTRL_30, 1 << 3, 0);
    
	MCP2515SetCanFilter(MCP_TXB0SIDH_31,Message->Ext,Message->Id_Add);	   // 31 32 33 34 写 ID 号
	len = Message->DLC;
	if(len > 8)   len = 8;
	i = len;
	if(Message->Remote) i = i | 0x40;
	MCP2515SetRegister(0x35,i); 											// 填写要发送的字节数
	for(i = 0;i < len;i++)
	{
		MCP2515SetRegister(0x36 + i,Message->Data[i]);
	}
	MCP2515ModifyRegister(MCP_TXB0CTRL_30,0x0b,0x0b);					   // 发送控制寄存器
}

/*
 *****************************************************************
 * 软件设置 MCP2515 进入 休眠 模式
 * 可以从外部(mcu)唤醒 也可以自动总线唤醒
 *****************************************************************
 */
void cSetMCP2515ToSleepMode(void)
{
	MCP2515SetCanCtrlMode(MODE_SLEEP_20);									// 进入休眠模式
}

/*
 ******************************************************************
 * 外部主动唤醒 MCP2515
 * 这个可以不用，因为退出休眠模式时 所有的 IC 全部初始化了一次
 ******************************************************************
 */
void cFWWakeMCP2515(void)
{
    MCP2515ModifyRegister(MCP_CANINTF,MCP_WAKIF, 0xff);									// 	
}

/*
 ******************************************************************
 * 错误检测,如果出现总线关闭
 ******************************************************************
 */
void MCP2515_ErrorCheck(void)
{
    uint8_t data;
    data = MCP2515ReadRegister(MCP_EFLG);
    
    if(data & MCP_ERR_FLAG) //出错了
    {
        cInitMCP2515();
    }
}


