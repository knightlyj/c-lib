#ifndef __MCP2515_REG_H
#define __MCP2515_REG_H
// Define MCP2515 register addresses
#define MCP_RXF0SIDH_00	0x00
#define MCP_RXF0SIDL	0x01
#define MCP_RXF0EID8	0x02
#define MCP_RXF0EID0	0x03
#define MCP_RXF1SIDH_04	0x04
#define MCP_RXF1SIDL	0x05
#define MCP_RXF1EID8	0x06
#define MCP_RXF1EID0	0x07
#define MCP_RXF2SIDH_08	0x08
#define MCP_RXF2SIDL	0x09
#define MCP_RXF2EID8	0x0A
#define MCP_RXF2EID0	0x0B
#define MCP_CANSTAT		0x0E		/* CAN状态寄存器，7-6 OPMOD - 3-1 ICOD - */
#define MCP_CANCTRL_0F		0x0F			
#define MCP_RXF3SIDH_10	0x10
#define MCP_RXF3SIDL	0x11
#define MCP_RXF3EID8	0x12
#define MCP_RXF3EID0	0x13
#define MCP_RXF4SIDH_14	0x14
#define MCP_RXF4SIDL	0x15
#define MCP_RXF4EID8	0x16
#define MCP_RXF4EID0	0x17
#define MCP_RXF5SIDH_18	0x18
#define MCP_RXF5SIDL	0x19
#define MCP_RXF5EID8	0x1A
#define MCP_RXF5EID0	0x1B
#define MCP_TEC			0x1C
#define MCP_REC			0x1D	   /* 接收错误计数器 */
#define MCP_RXM0SIDH_20	0x20	   /* 验收屏蔽寄存器0，标准ID高位 */
#define MCP_RXM0SIDL	0x21	   /* 验收屏蔽寄存器0，标准ID低位 */
#define MCP_RXM0EID8	0x22	   /* 验收屏蔽寄存器0，扩展ID高位 */
#define MCP_RXM0EID0	0x23	   /* 验收屏蔽寄存器0，扩展ID低位 */
#define MCP_RXM1SIDH_24	0x24	   /* 验收屏蔽寄存器1，标准ID高位 */
#define MCP_RXM1SIDL	0x25	   /* 验收屏蔽寄存器1，标准ID低位 */
#define MCP_RXM1EID8	0x26	   /* 验收屏蔽寄存器1，标准ID高位 */
#define MCP_RXM1EID0	0x27	   /* 验收屏蔽寄存器1，扩展ID低位 */
#define MCP_CNF3_28		0x28		////* 配置寄存器3 */
#define MCP_CNF2_29		0x29		////* 配置寄存器2 */
#define MCP_CNF1_2A		0x2A		////* 配置寄存器1 */
#define MCP_CANINTE_2B		0x2B		/* CAN中断全能寄存器 */
#define MCP_CANINTF		0x2C		/* CAN中断标志寄存器 */    
#define MCP_EFLG		0x2D
#define MCP_TXB0CTRL_30	0x30		/* 发送缓冲区0控制寄存器 */
#define MCP_TXB1CTRL_40	0x40		/* 发送缓冲区1控制寄存器 */
#define MCP_TXB2CTRL_50	0x50		/* 发送缓冲区2控制寄存器 */
#define MCP_RXB0CTRL_60	0x60		/* 接收缓冲区0控制寄存器 */
#define MCP_RXB0SIDH	0x61
#define MCP_RXB1CTRL_70	0x70		/* 接收缓冲区0控制寄存器 */
#define MCP_RXB1SIDH_71	0x71
#define	MCP_TXB0SIDH_31	0x31
#define	MCP_TXB1SIDH	0x41
#define	MCP_TXB2SIDH	0x51


#define MCP_TX_INT		0x1C		// Enable all transmit interrupts
#define MCP_TX01_INT	0x0C		// Enable TXB0 and TXB1 interrupts
#define MCP_RX_INT		0x03		// Enable receive interrupts
#define MCP_NO_INT		0x00		// Disable all interrupts

#define MCP_TX01_MASK	0x14
#define MCP_TX_MASK		0x54

/*************************************************************/
// Define SPI Instruction Set
#define MCP_WRITE		0x02	   /* MCP写指令代码 */

#define MCP_READ		0x03

#define MCP_BITMOD		0x05	   /* 位修改命令 */
/*************************************************************/
#define MCP_LOAD_TX0	0x40
#define MCP_LOAD_TX1	0x42
#define MCP_LOAD_TX2	0x44

#define MCP_RTS_TX0		0x81
#define MCP_RTS_TX1		0x82
#define MCP_RTS_TX2		0x84
#define MCP_RTS_ALL		0x87

#define MCP_READ_RX0	0x90
#define MCP_READ_RX1	0x94

#define MCP_READ_STATUS	0xA0

#define MCP_RX_STATUS	0xB0

#define MCP_RESET		0xC0
// CANCTRL Register Values
#define MODE_NORMAL_00	   	0x00
#define MODE_SLEEP_20		0x20
#define MODE_LOOPBACK_40	0x40
#define MODE_LISTENONLY_60 	0x60
#define MODE_CONFIG_80	   	0x80

#define MODE_POWERUP	0xE0
#define MODE_MASK		0xE0				 /* 模式掩码，只看高三位 */
#define ABORT_TX		0x10
#define MODE_ONESHOT	0x08
#define CLKOUT_ENABLE	0x04
#define CLKOUT_DISABLE	0x00
#define CLKOUT_PS1		0x00
#define CLKOUT_PS2		0x01
#define CLKOUT_PS4		0x02
#define CLKOUT_PS8		0x03
// CNF1 Register Values
#define SJW1			0x00			  /* 1TQ */
#define SJW2			0x40			  /* 2TQ */
#define SJW3			0x80			  /* 3TQ */
#define SJW4			0xC0			  /* 4TQ */
// CNF2 Register Values
#define BTLMODE			0x80			  /* PHASE2时间由CNF3寄存器设定 */
#define SAMPLE_1X		0x00			  /* 单次采样 */
#define SAMPLE_3X		0x40
// CNF3 Register Values
#define SOF_ENABLE		0x80
#define SOF_DISABLE		0x00			  /* 当CANCTRAL.CLKEN为1时，CLKOUT引脚作时间引脚 */
#define WAKFIL_ENABLE	0x40
#define WAKFIL_DISABLE	0x00			  /* 唤醒滤波器不使能 */
// CANINTF Register Bits
#define MCP_RX0IF		0x01
#define MCP_RX1IF		0x02
#define MCP_TX0IF		0x04
#define MCP_TX1IF		0x08
#define MCP_TX2IF		0x10
#define MCP_ERRIF		0x20
#define MCP_WAKIF		0x40
#define MCP_MERRF		0x80
// CANINTE Register Bits
#define MCP_RX0IE		0x01		   /* 接收缓冲器0满中断使能 */
#define MCP_RX1IE		0x02		   /* 接收缓冲器1满中断使能 */
#define MCP_TX0IE		0x04		   /* 发送缓冲器0空中断使能 */
#define MCP_TX1IE		0x08		   /* 发送缓冲器1空中断使能 */
#define MCP_TX2IE		0x10		   /* 发送缓冲器2空中断使能 */
#define MCP_ERRIE		0x20
#define MCP_WAKIE		0x40
#define MCP_MERRE		0x80

#define MCP_RX1_OVR     0x80
#define MCP_ERR_FLAG    0x38
#endif
/******************************************************************/

