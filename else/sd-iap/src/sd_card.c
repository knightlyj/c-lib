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

//SPI����ʱ
static void SD_SPI_Delay(int x)
{
    uint32_t i,k;
    for(i =0; i < x; i++)
    {
        k = 0;
        while(k++ < 10);
    }
}

//��1Byte,�����ؽ��յ�������
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
        (byte&0x80) ? MOSI_High() : MOSI_Low();     // ��Ŀ�������������
        CLK_High();
        SD_SPI_Delay(1);
        Dat |= Read_MISO();
        byte<<=1;
        SD_SPI_Delay(1);
    }

    CS_High();
    SD_SPI_Delay(1);
    MOSI_High();                                    //  �ͷ�MOSI
    return Dat;
}

//��1Byte,�����ؽ��յ�������,����û����ʱ
uint8_t SD_SPI_SendFast(uint8_t byte)
{
    uint8_t i, Dat = 0;

    CS_Low();
    for(i=0; i<8; i++)
    {
        Dat<<=1;
        CLK_Low();
        (byte&0x80)? MOSI_High() : MOSI_Low();      // ��Ŀ�������������
        CLK_High();
        Dat |= Read_MISO();
        byte<<=1;
    }

    CS_High();

    MOSI_High();                                    // �ͷ�MOSI
    return Dat;
}

//�ȴ���׼����
//����ֵ:0,׼������;����,�������
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
    SD_SPI_Send(0xff);//�ṩ�����8��ʱ��
}

//�ȴ���׼����
//����ֵ:0,׼������;����,�������
u8 SD_Select(void)
{
    CS_Low();
    if(SD_WaitReady()==0)return 0;//OK
    SD_Deselect();
    return 1;//�0�8�0�6�0�7�0�5�0�8���0�5
}


//�ȴ�SD����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ
//    ����,�õ���Ӧֵʧ��
u8 SD_GetResponse(u8 Response)
{
    u16 Count=0xFFFF;//�ȴ�����	  
    while ((SD_SPI_Send(0XFF)!=Response)&&Count)Count--;//�ȴ��õ�׼ȷ�Ļ�Ӧ  	 
    if (Count==0)   return MSD_RESPONSE_FAILURE;    // �õ���Ӧʧ��   
    else                    return MSD_RESPONSE_NO_ERROR;   // ��ȷ��Ӧ
}

//��SD������һ������
//����: u8 cmd   ���� 
//      u32 arg  �������
//      u8 crc   crcУ��ֵ	   
//����ֵ:SD�����ص���Ӧ	
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;
    u8 Retry=0;
    SD_Deselect();//ȡ���ϴ�Ƭѡ
    if(SD_Select())return 0XFF;//ƬѡʧЧ 
    //����
    SD_SPI_Send(cmd | 0x40);//�ֱ�д������
    SD_SPI_Send(arg >> 24);
    SD_SPI_Send(arg >> 16);
    SD_SPI_Send(arg >> 8);
    SD_SPI_Send(arg);
    SD_SPI_Send(crc);
    if(cmd==CMD12)SD_SPI_Send(0xff);//Skip a stuff byte when stop reading
    //�ȴ���Ӧ����ʱ�˳�
    Retry=0X1F;
    do
    {
        r1=SD_SPI_Send(0xFF);
    }
    while((r1&0X80) && Retry--);
    //����״ֵ̬
    return r1;
}


// ��sd����ȡһ�����ݰ�������
// buf:���ݻ�����
// len:Ҫ��ȡ�����ݳ���.
// ����ֵ:0,�ɹ�;����,ʧ��;
u8 SD_RecvData(u8*buf,u16 len)
{
    if(SD_GetResponse(0xFE))
        return 1;//�ȴ�SD������������ʼ����0xFE
    while(len--)//��ʼ��������
    {
        *buf=SD_SPI_Send(0xFF);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    SD_SPI_Send(0xFF);
    SD_SPI_Send(0xFF);
    return 0;//��ȡ�ɹ�
}

u8  SD_Type=0;//SD��������
uint8_t SD_InitCard()
{
    u8 r1;      // ����ֵ
    u16 retry;  // ��ʱ����
    u8 buf[4];
    u16 i;

    //��������74������
    for(i=0; i<10; i++) 
        SD_SPI_Send(0XFF); 
    retry=20;
    do
    {
        r1=SD_SendCmd(CMD0,0,0x95);//����IDLE״̬
    }
    while((r1!=0X01) && retry--);
    SD_Type=0;//Ĭ���޿�
    if(r1==0X01)
    {
        if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
        {
            for(i=0; i<4; i++)
            {
                buf[i]=SD_SPI_Send(0XFF); //Get trailing return value of R7 resp
            }
            if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
            {
                retry=0XFFFE;
                do
                {
                    SD_SendCmd(CMD55,0,0X01);   //����CMD55
                    r1=SD_SendCmd(CMD41,0x40000000,0X01);//����CMD41
                }
                while(r1&&retry--);
                if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//����SD2.0���汾��ʼ
                {
                    for(i=0; i<4; i++)buf[i]=SD_SPI_Send(0XFF); //�õ�OCRֵ
                    if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC; //���CCS
                    else SD_Type=SD_TYPE_V2;
                }
            }
        }
        else //SD V1.x/ MMC V3
        {
            SD_SendCmd(CMD55,0,0X01);       //����CMD55
            r1=SD_SendCmd(CMD41,0,0X01);    //����CMD41
            if(r1<=1)
            {
                SD_Type=SD_TYPE_V1;
                retry=0XFFFE;
                do //�ȴ��˳�IDLEģʽ
                {
                    SD_SendCmd(CMD55,0,0X01);   //����CMD55
                    r1=SD_SendCmd(CMD41,0,0X01);//����CMD41
                }
                while(r1&&retry--);
            }
            else
            {
                SD_Type=SD_TYPE_MMC;//MMC V3
                retry=0XFFFE;
                do //�ȴ��˳�IDLEģʽ
                {
                    r1=SD_SendCmd(CMD1,0,0X01);//����CMD1
                }
                while(r1&&retry--);
            }
            if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)
                SD_Type=SD_TYPE_ERR; //����Ŀ�
        }
    }
    SD_Deselect();//ȡ��Ƭѡ
    if(SD_Type)
    {
        Log("sd type: %d\r\n", SD_Type);
        return 0;
    }
    else if(r1)
        return r1;

    return 0xaa;//��������
}


//��SD��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
u8 SD_ReadSectors(u8*buf,u32 sector,u8 cnt)
{
    u8 r1;
    if(SD_Type!=SD_TYPE_V2HC)
        sector <<= 9; //ת��Ϊ�ֽڵ�ַ
    if(cnt==1)
    {
        r1=SD_SendCmd(CMD17,sector,0X01);//������
        if(r1==0)//ָ��ͳɹ�
        {
            r1=SD_RecvData(buf,512);//����512���ֽ�
        }
    }
    else
    {
        r1=SD_SendCmd(CMD18,sector,0X01);//����������
        do
        {
            r1=SD_RecvData(buf,512);//����512���ֽ�
            buf+=512;
        }
        while(--cnt && r1==0);
        SD_SendCmd(CMD12,0,0X01);   //����ֹͣ����
    }
    SD_Deselect();//ȡ��Ƭѡ
    return r1;//
}



