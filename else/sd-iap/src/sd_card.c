#include "sd_card.h"



#include "stdio.h"

//------------CS-------------------
CL_INLINE static void CS_Low()
{
    GPIO_ResetBits(SPI_CS_PORT_GROUP, SPI_CS_PORT_NUM);
}

CL_INLINE static void CS_High()
{
    GPIO_SetBits(SPI_CS_PORT_GROUP, SPI_CS_PORT_NUM);
}

//-------------MOSI---------------------
CL_INLINE static void MOSI_Low()
{
    GPIO_ResetBits(SPI_MOSI_PORT_GROUP, SPI_MOSI_PORT_NUM);
}

CL_INLINE static void MOSI_High()
{
    GPIO_SetBits(SPI_MOSI_PORT_GROUP, SPI_MOSI_PORT_NUM);
}

//-------------CLK-------------------
CL_INLINE static void CLK_Low()
{
    GPIO_ResetBits(SPI_CLK_PORT_GROUP, SPI_CLK_PORT_NUM);
}

CL_INLINE static void CLK_High()
{
    GPIO_SetBits(SPI_CLK_PORT_GROUP, SPI_CLK_PORT_NUM);
}

//---------------MISO-------------------
CL_INLINE static uint8_t Read_MISO()
{
    return GPIO_ReadInputDataBit(SPI_MISO_PORT_GROUP, SPI_MISO_PORT_NUM);
}


void SD_ModuleInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //CS
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Pin = SPI_CS_PORT_NUM;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SPI_CS_PORT_GROUP, &GPIO_InitStructure);

    //CLK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = SPI_CLK_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_CLK_PORT_GROUP, &GPIO_InitStructure);

    //MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = SPI_MISO_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_MISO_PORT_GROUP, &GPIO_InitStructure);

    //MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_MOSI_PORT_GROUP, &GPIO_InitStructure);

    //card detect
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = SD_DETECT_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SD_DETECT_PORT_GROUP, &GPIO_InitStructure);

    CS_High();
    MOSI_High();
    CLK_High();
}

void SD_ModuleExit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //CS
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_Pin = SPI_CS_PORT_NUM;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SPI_CS_PORT_GROUP, &GPIO_InitStructure);

    //CLK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = SPI_CLK_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_CLK_PORT_GROUP, &GPIO_InitStructure);

    //MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PORT_NUM;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_MOSI_PORT_GROUP, &GPIO_InitStructure);
}

//SPI的延时
static void SD_SPI_Delay(int x)
{
    uint32_t i,k;
    for(i =0; i < x; i++)
    {
        k = 0;
        while(k++ < 10);
    }
}

//发1Byte,并返回接收到的数据
uint8_t SD_SPI_Send(uint8_t byte)
{
    uint8_t i, Dat = 0;

    CS_Low();
    SD_SPI_Delay(1);
    for(i=0; i<8; i++)
    {
        Dat<<=1;
        CLK_Low();
        SD_SPI_Delay(1);
        (byte&0x80) ? MOSI_High() : MOSI_Low();     // 三目运算符传输数据
        CLK_High();
        SD_SPI_Delay(1);
        Dat |= Read_MISO();
        byte<<=1;
        SD_SPI_Delay(1);
    }

    CS_High();
    SD_SPI_Delay(1);
    MOSI_High();                                    //  释放MOSI
    return Dat;
}

//发1Byte,并返回接收到的数据,但是没有延时
uint8_t SD_SPI_SendFast(uint8_t byte)
{
    uint8_t i, Dat = 0;

    CS_Low();
    for(i=0; i<8; i++)
    {
        Dat<<=1;
        CLK_Low();
        (byte&0x80)? MOSI_High() : MOSI_Low();      // 三目运算符传输数据
        CLK_High();
        Dat |= Read_MISO();
        byte<<=1;
    }

    CS_High();

    MOSI_High();                                    // 释放MOSI
    return Dat;
}

//等待卡准备好
//返回值:0,准备好了;其他,错误代码
u8 SD_WaitReady(void)
{
    u32 t=0;
    do
    {
        if(SD_SPI_Send(0XFF)==0XFF)return 0;//OK
        t++;
    }
    while(t<0XFFFFFF); //????????
    return 1;
}

void SD_Deselect(void)
{
    CS_High();
    SD_SPI_Send(0xff);//提供额外的8个时钟
}

//等待卡准备好
//返回值:0,准备好了;其他,错误代码
u8 SD_Select(void)
{
    CS_Low();
    if(SD_WaitReady()==0)return 0;//OK
    SD_Deselect();
    return 1;//µÈ´ýÊ§°Ü
}


//等待SD卡回应
//Response:要得到的回应值
//返回值:0,成功得到了该回应值
//    其他,得到回应值失败
u8 SD_GetResponse(u8 Response)
{
    u16 Count=0xFFFF;//等待次数	  
    while ((SD_SPI_Send(0XFF)!=Response)&&Count)Count--;//等待得到准确的回应  	 
    if (Count==0)   return MSD_RESPONSE_FAILURE;    // 得到回应失败   
    else                    return MSD_RESPONSE_NO_ERROR;   // 正确回应
}

//向SD卡发送一个命令
//输入: u8 cmd   命令 
//      u32 arg  命令参数
//      u8 crc   crc校验值	   
//返回值:SD卡返回的响应	
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;
    u8 Retry=0;
    SD_Deselect();//取消上次片选
    if(SD_Select())return 0XFF;//片选失效 
    //发送
    SD_SPI_Send(cmd | 0x40);//分别写入命令
    SD_SPI_Send(arg >> 24);
    SD_SPI_Send(arg >> 16);
    SD_SPI_Send(arg >> 8);
    SD_SPI_Send(arg);
    SD_SPI_Send(crc);
    if(cmd==CMD12)SD_SPI_Send(0xff);//Skip a stuff byte when stop reading
    //等待响应，或超时退出
    Retry=0X1F;
    do
    {
        r1=SD_SPI_Send(0xFF);
    }
    while((r1&0X80) && Retry--);
    //返回状态值
    return r1;
}


// 从sd卡读取一个数据包的内容
// buf:数据缓存区
// len:要读取的数据长度.
// 返回值:0,成功;其他,失败;
u8 SD_RecvData(u8*buf,u16 len)
{
    if(SD_GetResponse(0xFE))
        return 1;//等待SD卡发回数据起始令牌0xFE
    while(len--)//开始接收数据
    {
        *buf=SD_SPI_Send(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    SD_SPI_Send(0xFF);
    SD_SPI_Send(0xFF);
    return 0;//读取成功
}

u8  SD_Type=0;//SD卡的类型
uint8_t SD_InitCard()
{
    u8 r1;      // 返回值
    u16 retry;  // 超时计数
    u8 buf[4];
    u16 i;

    //发送最少74个脉冲
    for(i=0; i<10; i++) 
        SD_SPI_Send(0XFF); 
    retry=20;
    do
    {
        r1=SD_SendCmd(CMD0,0,0x95);//进入IDLE状态
    }
    while((r1!=0X01) && retry--);
    SD_Type=0;//默认无卡
    if(r1==0X01)
    {
        if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
        {
            for(i=0; i<4; i++)
            {
                buf[i]=SD_SPI_Send(0XFF); //Get trailing return value of R7 resp
            }
            if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
            {
                retry=0XFFFE;
                do
                {
                    SD_SendCmd(CMD55,0,0X01);   //发送CMD55
                    r1=SD_SendCmd(CMD41,0x40000000,0X01);//发送CMD41
                }
                while(r1&&retry--);
                if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
                {
                    for(i=0; i<4; i++)buf[i]=SD_SPI_Send(0XFF); //得到OCR值
                    if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC; //检查CCS
                    else SD_Type=SD_TYPE_V2;
                }
            }
        }
        else //SD V1.x/ MMC V3
        {
            SD_SendCmd(CMD55,0,0X01);       //发送CMD55
            r1=SD_SendCmd(CMD41,0,0X01);    //发送CMD41
            if(r1<=1)
            {
                SD_Type=SD_TYPE_V1;
                retry=0XFFFE;
                do //等待退出IDLE模式
                {
                    SD_SendCmd(CMD55,0,0X01);   //发送CMD55
                    r1=SD_SendCmd(CMD41,0,0X01);//发送CMD41
                }
                while(r1&&retry--);
            }
            else
            {
                SD_Type=SD_TYPE_MMC;//MMC V3
                retry=0XFFFE;
                do //等待退出IDLE模式
                {
                    r1=SD_SendCmd(CMD1,0,0X01);//发送CMD1
                }
                while(r1&&retry--);
            }
            if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)
                SD_Type=SD_TYPE_ERR; //错误的卡
        }
    }
    SD_Deselect();//取消片选
    if(SD_Type)
    {
        Log("sd type: %d\r\n", SD_Type);
        return 0;
    }
    else if(r1)
        return r1;

    return 0xaa;//其他错误
}


//读SD卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
u8 SD_ReadSectors(u8*buf,u32 sector,u8 cnt)
{
    u8 r1;
    if(SD_Type!=SD_TYPE_V2HC)
        sector <<= 9; //转换为字节地址
    if(cnt==1)
    {
        r1=SD_SendCmd(CMD17,sector,0X01);//读命令
        if(r1==0)//指令发送成功
        {
            r1=SD_RecvData(buf,512);//接收512个字节
        }
    }
    else
    {
        r1=SD_SendCmd(CMD18,sector,0X01);//连续读命令
        do
        {
            r1=SD_RecvData(buf,512);//接收512个字节
            buf+=512;
        }
        while(--cnt && r1==0);
        SD_SendCmd(CMD12,0,0X01);   //发送停止命令
    }
    SD_Deselect();//取消片选
    return r1;//
}



