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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;               // SDA 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_SDA_PORT_GROUP, &GPIO_InitStructure);
}

CL_INLINE static void SDA_IN()
{
    GPIO_InitTypeDef GPIO_InitStructure;
       
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PORT_NUM;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;               // SDA 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_SDA_PORT_GROUP, &GPIO_InitStructure);
}


/*
 *********************************************************
 * 初始化IIC
 *********************************************************
 */
void IIC_Init(void)
{                        
    GPIO_InitTypeDef GPIO_InitStructure;
       
//    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PORT_NUM;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;               // SDA 推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(IIC_SDA_PORT_GROUP, &GPIO_InitStructure);
    SDA_OUT();

    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PORT_NUM;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;               // SCL 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_SCL_PORT_GROUP, &GPIO_InitStructure);

    SCL_High();
    SDA_High();
}

/*
 *********************************************************
 * 产生IIC起始信号
 *********************************************************
 */
void IIC_Start(void)
{
    SDA_OUT();                                                      // sda线输出
    SDA_High();        
    SCL_High();
    DelayUs(4);
    SDA_Low();                                                    // START:when CLK is high,DATA change form high to low 
    DelayUs(4);
    SCL_Low();                                                    // 钳住I2C总线，准备发送或接收数据 
}     

/*
 *********************************************************
 * 产生IIC停止信号
 *********************************************************
 */
void IIC_Stop(void)
{
    SDA_OUT();                                                      // sda线输出
    SCL_Low();
    SDA_Low();                                                    // STOP:when CLK is high DATA change form low to high
    DelayUs(4);
    SCL_High(); 
    SDA_High();                                                    // 发送I2C总线结束信号
    DelayUs(4);                                
}

/*
 *********************************************************
 * 等待应答信号到来
 * 返回值：1，接收应答失败
 *         0，接收应答成功
 *********************************************************
 */
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    
    SDA_IN();                                                       // SDA设置为输入  
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
    SCL_Low();                                                    // 时钟输出0      
    return 0;  
} 

/*
 **********************************************************
 * 产生ACK应答
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
 * 不产生ACK应答 
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
 * IIC发送一个字节
 * 返回从机有无应答
 * 1，有应答
 * 0，无应答  
 **********************************************************  
 */       
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    SDA_OUT();      
    SCL_Low();                                                // 拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7)
            SDA_High();
        else
            SDA_Low();
        txd <<= 1;      
        DelayUs(2);                                            // 对TEA5767这三个延时都是必须的
        SCL_High();
        DelayUs(2); 
        SCL_Low();  
        DelayUs(2);
    }    
}       

/*
 **************************************************************
 * 读1个字节，ack=1时，发送ACK，ack=0，发送nACK  
 ************************************************************** 
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive = 0;
    
    SDA_IN();                                                   // SDA设置为输入
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
        IIC_NAck();                                             // 发送nACK
    else
        IIC_Ack();                                              // 发送ACK   
    return receive;
}



























