#ifndef _CAR_XXX_2515H_  
#define _CAR_XXX_2515H_    
	

    #define MCP2515_USE_FILTER      //�Ƿ��˲�

	#define _MARK1_ID				0x140					   	// ��λ ID						// �˲� / ���μĴ�����ֵ��                               
	#define _MARK2_ID				222					   	// ��λ ID                      // ����λ n   ����λ n    ���ı�ʶ��λ   ���ܻ�ܾ�λn   
	#define _MARK3_ID				0x12A					   	// ��λ ID                      //  0            x             x              ����       
	#define _MARK4_ID				0x1F1						// ��λ ID                      //  1		     0             0           	  ����       
                                                                                                //  1            0             1              �ܾ�       
	/* ֻ�� 4 �� ID ���˱�ʶ��*/                                                            	//	1		     1             0 			  �ܾ�	     
	#define _MCP2515_M1_FAMT		_MCP2515_CAN_STD											// 	1            1             1              ����       
	#define _MCP2515_M1_ID			0x7FF						// �ص��� �����˲���         	
                                                                                                
	#define _MCP2515_ID3			_MARK1_ID					// �� 4 �� ���˱�ʶ��������	
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
	const CAN_SPIMessage CANTxMessage1 = {0x34F,CAN_ID_STD,CAN_RTR_DATA,8,0x10,0x12,0x13,0x14,0x15,0x16,0x17,0x18};  	// ϵͳ���� 
	const CAN_SPIMessage CANTxMessage2 = {0x34F56,CAN_ID_EXT,CAN_RTR_DATA,8,0x20,0x22,0x23,0x24,0x25,0x26,0x27,0x28};  	// ϵͳ���� 
#endif
extern const CAN_SPIMessage CANTxMessage1;
extern const CAN_SPIMessage CANTxMessage2;

#endif

