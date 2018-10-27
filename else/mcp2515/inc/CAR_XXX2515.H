#ifndef _CAR_XXX_2515H_  
#define _CAR_XXX_2515H_    
	

    #define MCP2515_USE_FILTER      //是否开滤波

	#define _MARK1_ID				0x140					   	// 档位 ID						// 滤波 / 屏蔽寄存器真值表                               
	#define _MARK2_ID				222					   	// 档位 ID                      // 屏蔽位 n   过滤位 n    报文标识符位   接受或拒绝位n   
	#define _MARK3_ID				0x12A					   	// 档位 ID                      //  0            x             x              接受       
	#define _MARK4_ID				0x1F1						// 档位 ID                      //  1		     0             0           	  接受       
                                                                                                //  1            0             1              拒绝       
	/* 只有 4 个 ID 过滤标识符*/                                                            	//	1		     1             0 			  拒绝	     
	#define _MCP2515_M1_FAMT		_MCP2515_CAN_STD											// 	1            1             1              接受       
	#define _MCP2515_M1_ID			0x7FF						// 关掉了 屏蔽滤波器         	
                                                                                                
	#define _MCP2515_ID3			_MARK1_ID					// 有 4 个 过滤标识符可设置	
	#define _MCP2515_ID3_FAMT		_MCP2515_CAN_STD                                            
                                                                                                
	#define _MCP2515_ID4			_MARK2_ID                                                   
	#define _MCP2515_ID4_FAMT		_MCP2515_CAN_EXT

	#define _MCP2515_ID5			_MARK3_ID
	#define _MCP2515_ID5_FAMT		_MCP2515_CAN_STD

	#define _MCP2515_ID6			_MARK4_ID
	#define _MCP2515_ID6_FAMT		_MCP2515_CAN_STD
	
		
extern void cCAN_MCPReceiveProcess(void);
extern void cTestMCP2515Transmit(void);

#ifdef _MCP2515_C_
	const CAN_SPIMessage CANTxMessage1 = {0x34F,CAN_ID_STD,CAN_RTR_DATA,8,0x10,0x12,0x13,0x14,0x15,0x16,0x17,0x18};  	// 系统激活 
	const CAN_SPIMessage CANTxMessage2 = {0x34F56,CAN_ID_EXT,CAN_RTR_DATA,8,0x20,0x22,0x23,0x24,0x25,0x26,0x27,0x28};  	// 系统激活 
#endif
extern const CAN_SPIMessage CANTxMessage1;
extern const CAN_SPIMessage CANTxMessage2;

#endif


