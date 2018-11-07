#include "cl_common.h"
#include "gpio_iic.h"
#include "helper.h"

CL_INLINE static void SCL_High()
{
    GPIO_SetBits(IIC_SCL_PORT_GROUP, IIC_SCL_PORT_NUM);
}

CL_INLINE static void SCL_Low()
{
    GPIO_ResetBits(IIC_SCL_PORT_GROUP, IIC_SCL_PORT_NUM);
}

CL_INLINE static void SDA_High()
{
    GPIO_SetBits(IIC_SDA_PORT_GROUP, IIC_SDA_PORT_NUM);
}

CL_INLINE static void SDA_Low()
{
    GPIO_ResetBits(IIC_SDA_PORT_GROUP, IIC_SDA_PORT_NUM);
}

static uint8_t ReadSDA(void)
{
    return GPIO_ReadInputDataBit(IIC_SDA_PORT_GROUP, IIC_SDA_PORT_NUM);
}

CL_INLINE static void SDA_OUT()
{
    GPIO_InitTypeDef GPIO_InitStructure;
       
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PORT_NUM;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;               // SDA �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_SDA_PORT_GROUP, &GPIO_InitStructure);
}

CL_INLINE static void SDA_IN()
{
    GPIO_InitTypeDef GPIO_InitStructure;
       
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PORT_NUM;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;               // SDA �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_SDA_PORT_GROUP, &GPIO_InitStructure);
}


/*
 *********************************************************
 * ��ʼ��IIC
 *********************************************************
 */
void IIC_Init(void)
{                        
    GPIO_InitTypeDef GPIO_InitStructure;
       
//    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PORT_NUM;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;               // SDA �������
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(IIC_SDA_PORT_GROUP, &GPIO_InitStructure);
    SDA_OUT();

    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PORT_NUM;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;               // SCL �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_SCL_PORT_GROUP, &GPIO_InitStructure);

    SCL_High();
    SDA_High();
}

/*
 *********************************************************
 * ����IIC��ʼ�ź�
 *********************************************************
 */
void IIC_Start(void)
{
    SDA_OUT();                                                      // sda�����
    SDA_High();        
    SCL_High();
    DelayUs(4);
    SDA_Low();                                                    // START:when CLK is high,DATA change form high to low 
    DelayUs(4);
    SCL_Low();                                                    // ǯסI2C���ߣ�׼�����ͻ�������� 
}     

/*
 *********************************************************
 * ����IICֹͣ�ź�
 *********************************************************
 */
void IIC_Stop(void)
{
    SDA_OUT();                                                      // sda�����
    SCL_Low();
    SDA_Low();                                                    // STOP:when CLK is high DATA change form low to high
    DelayUs(4);
    SCL_High(); 
    SDA_High();                                                    // ����I2C���߽����ź�
    DelayUs(4);                                
}

/*
 *********************************************************
 * �ȴ�Ӧ���źŵ���
 * ����ֵ��1������Ӧ��ʧ��
 *         0������Ӧ��ɹ�
 *********************************************************
 */
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    
    SDA_IN();                                                       // SDA����Ϊ����  
    SDA_High();DelayUs(1);     
    SCL_High();DelayUs(1);   
    while(ReadSDA())
    {
        ucErrTime ++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    SCL_Low();                                                    // ʱ�����0      
    return 0;  
} 

/*
 **********************************************************
 * ����ACKӦ��
 **********************************************************
 */
void IIC_Ack(void)
{
    SCL_Low();
    SDA_OUT();
    SDA_Low();
    DelayUs(2);
    SCL_High();
    DelayUs(2);
    SCL_Low();
}

/*
 **********************************************************
 * ������ACKӦ�� 
 **********************************************************    
 */    
void IIC_NAck(void)
{
    SCL_Low();
    SDA_OUT();
    SDA_High();
    DelayUs(2);
    SCL_High();
    DelayUs(2);
    SCL_Low();
}   
           
/*                         
 **********************************************************            
 * IIC����һ���ֽ�
 * ���شӻ�����Ӧ��
 * 1����Ӧ��
 * 0����Ӧ��  
 **********************************************************  
 */       
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    SDA_OUT();      
    SCL_Low();                                                // ����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7)
            SDA_High();
        else
            SDA_Low();
        txd <<= 1;      
        DelayUs(2);                                            // ��TEA5767��������ʱ���Ǳ����
        SCL_High();
        DelayUs(2); 
        SCL_Low();  
        DelayUs(2);
    }    
}       

/*
 **************************************************************
 * ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK  
 ************************************************************** 
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive = 0;
    
    SDA_IN();                                                   // SDA����Ϊ����
    for(i = 0; i < 8; i++ )
    {
        SCL_Low(); 
        DelayUs(2);
        SCL_High();
        receive <<= 1;
        if(ReadSDA())    receive++;   
        DelayUs(1); 
    }                    
    if (!ack)
        IIC_NAck();                                             // ����nACK
    else
        IIC_Ack();                                              // ����ACK   
    return receive;
}



























