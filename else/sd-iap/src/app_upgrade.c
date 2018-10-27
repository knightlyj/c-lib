#include "app_upgrade.h"
#include "flash_layout.h"
#include "stm32f10x_crc.h"
#include "hl_iap.h"
#include "ff.h"
#include "sd_card.h"
#include "global_variables.h"
#include "led.h"
#include "string.h"
#include "iwdg.h"

#include "stdio.h"

#define READ_BYTES_ONCE 512


FATFS fs0;
FILINFO fno;
static FATFS fs0; //File system
static AppInfo_t appInfo; //app������Ϣ

//��ȡapp����hash
CL_INLINE static uint32_t GetAppHash()
{
    CRC_ResetDR();
    return CRC_CalcBlockCRC((uint32_t*)APP_START_ADDR, appInfo.size / 4);
}

//��flash��app��Ϣ
CL_INLINE static void ReadAppInfo()
{
    memcpy(&appInfo, (const void*)DAT_START_ADDR, sizeof(appInfo));
//    if(appInfo.size >= APP_SIZE)
//        appInfo.size = APP_SIZE;
}


//����Ƿ�����Ч��bin�ļ�
CL_BOOL HasValidBinFile()
{
    FRESULT res;
    FIL file;
    uint8_t fileHead[8];
    UINT readBytes;
    DIR dir;
    FILINFO fno;
    
    //1.�ж��Ƿ���Ҫ����
    res = f_mount(&fs0, "0:/", 1);
    if(res != FR_OK)
        goto out;   //����ʧ��

    res = f_opendir(&dir, IAP_BIN_FILE_DIR);
    if(res != FR_OK)
        goto out;   //��Ŀ¼ʧ��

    //�ҵ�Ŀ���ļ���Ϣ,����ļ���С
    for (;;) {
		res = f_readdir(&dir, &fno);                   /* Read a directory item */
		if (res != FR_OK || fno.fname[0] == 0) 
        {  //��ȡ����   
            res = FR_DISK_ERR;
			f_closedir(&dir);
            goto out;
        }
        
		if (fno.fattrib & AM_DIR) 
        {    
			continue;   //�Ǹ�Ŀ¼,����
		}
		else 
        {                                       /* It is a file. */
            if(strcasecmp(IAP_BIN_FILE_NAME, fno.fname) == 0)
            {   //�ļ���ƥ��,��Ŀ���ļ�
                if(fno.fsize > APP_SIZE)
                {   //�ļ�����,����
                    res = FR_DISK_ERR;
			        f_closedir(&dir);
                    goto out;
                }
                else
                {   //�ļ���С����,����ѭ��
                    break;
                }
            }
            else
            {
                continue;
            }
		}
	}

    f_closedir(&dir);

    res = f_open(&file, IAP_BIN_FILE_PATH, FA_READ); //open file
    if(res != FR_OK)
        goto out;  //���ļ�ʧ��

    //TODO: �ļ���С���
    //res = f_lseek(&file, 0);

    res = f_read(&file, fileHead, CL_ARRAY_LENGTH(fileHead), &readBytes); //read file;
    if(res != FR_OK)
        goto close_file; //��ȡ�ļ�ʧ��

    if(readBytes != CL_ARRAY_LENGTH(fileHead)) 
    {   //��ȡ���ȴ���
        res = FR_DISK_ERR;
        goto close_file;
    }

    if(fileHead[3] != 0x20 || fileHead[7] != 0x08)
    {   //�ļ�ͷ�����ݴ���
        res = FR_DISK_ERR;
        goto close_file;
    }

close_file:
    f_close(&file);
out:
    if(res != FR_OK)
        return CL_FALSE;
    
    return CL_TRUE;
}

static uint8_t readBuff[READ_BYTES_ONCE]; //���ļ���buff,Ӳ����Ϊ512��
//bin����д��app��
CL_RESULT FromBinToApp()
{
    FIL file;
    UINT readBytes;
    uint32_t totalReadyBytes = 0;
    FRESULT res;
    CL_BOOL done = CL_FALSE;
    uint32_t appHash;

    appInfo.size = 0;
    appInfo.hash = 0;

    CRC_ResetDR();

    res = f_open(&file, IAP_BIN_FILE_PATH, FA_READ); //open file
    if(res != FR_OK)
        goto out;  //���ļ�ʧ��

    IWDG_Feed(); //��������ʱ��ϳ�,��ι��

    //����data����app��Ϣ
    HL_IAP_Enable(DAT_START_ADDR, HL_IAP_PAGE_SIZE);
    HL_IAP_Erase(DAT_START_ADDR, HL_IAP_PAGE_SIZE);

    //����APP��
    HL_IAP_Enable(APP_START_ADDR, APP_SIZE); //����iap
    HL_IAP_Erase(APP_START_ADDR, APP_SIZE); //����flash

    
    while(1)
    {
        res = f_read(&file, readBuff, CL_ARRAY_LENGTH(readBuff), &readBytes);
        if(res != FR_OK)
            goto disable_iap;  //��ȡʧ��

        if((readBytes % 4) != 0) //��ȡ���ĳ���Ӧ����4������
        {
            res = FR_DISK_ERR;
            goto disable_iap;   
        }

        if(f_eof(&file))
        {   //���ļ���β��,����û����buff
            done = CL_TRUE;
        }
        else
        {   //û���ļ���β,Ӧ�ö�����buff
            if(readBytes != CL_ARRAY_LENGTH(readBuff))
            {
                res = FR_DISK_ERR;
                goto disable_iap;
            }
            
        }

        if(totalReadyBytes + readBytes > APP_SIZE)
        {
            res = FR_DISK_ERR;
            goto disable_iap;
        }

        //��ȡû����,д�뵽flash
        HL_IAP_Write(APP_START_ADDR + totalReadyBytes, readBuff, readBytes);
        appInfo.hash = CRC_CalcBlockCRC((uint32_t*)readBuff, readBytes / 4);
        
        totalReadyBytes += readBytes;

        IWDG_Feed(); //�м�Ҫι��,����ʱ��λ

        if(done == CL_TRUE)
            break;
        
    }
    
    appInfo.size = totalReadyBytes;

    appHash = GetAppHash();
    if(appHash != appInfo.hash)
    {
        res = FR_DISK_ERR;
        goto disable_iap;
    }

    //��app��Ϣ����������
    HL_IAP_Write(DAT_START_ADDR, (const uint8_t*)(&appInfo), sizeof(appInfo));
    
disable_iap:
    HL_IAP_Disable(APP_START_ADDR, APP_SIZE); //�ر�iap
    HL_IAP_Disable(DAT_START_ADDR, HL_IAP_PAGE_SIZE);

    f_close(&file);
out:
    if(res != FR_OK)
        return CL_FAILED;
        
    return CL_SUCCESS;
}

//app���Ƿ���Ч
CL_BOOL IsAppValid()
{
    uint32_t appHash;
    //app ͷ�����
    uint32_t* pAppHead = (uint32_t*)APP_START_ADDR;
    if((pAppHead[0] & 0xff000000) != 0x20000000
        || (pAppHead[1] & 0xff000000) != 0x08000000)
        return CL_FALSE;

    //������������app��Ϣ
    ReadAppInfo();
    
    //���app��Ϣ��ΪFFFF,������ǳ�����¼,APPҲ�ж�Ϊ��Ч
    if(appInfo.hash == 0xffffffff &&
        appInfo.size == 0xffffffff)
        return CL_TRUE;
    
    //�ж�hash
    appHash = GetAppHash();
    if(appHash != appInfo.hash)
        return CL_FALSE;
    
    return CL_TRUE;
}




//��ʼ��
void AppUpgrade_Init(void)
{
    //sdģ���ʼ��
    SD_ModuleInit();

    //��crcʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

}

//�˳�
void AppUpgrade_Exit(void)
{
    SD_ModuleExit();
    //��crcʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, DISABLE);
}



static CL_RESULT Upgrade(void)
{
    if(HasValidBinFile() == CL_TRUE)   //����Ƿ��п���bin�ļ�
    {   //�п��õ�bin�ļ�
        if(FromBinToApp() == CL_SUCCESS)
        {   //��bin�ļ�д��app��
            return CL_SUCCESS;
        }
        else
        {
            return CL_FAILED;
        }
    }
    else
    {
        Log("û�з���Ҫ����ļ�\r\n");
        return CL_FAILED;
    }
        
}


static uint32_t cardStableTime = 0; //��λ��ms
//����1��ʾ�ȶ�,2��ʾ���ڵȴ��ȶ�,0��ʾ���γ�
static int CardStable(uint16_t interval)
{
    if(SD_CheckCard())
    {
        cardStableTime += interval;
        if(cardStableTime > 2000)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        cardStableTime = 0;
        return 0;
    }
}


typedef enum
{
    AU_PowerUp = 0,   //�ϵ�
    AU_WaitCardStable,   //�ȴ�SD���ȶ�
    AU_WriteToApp,   //д��app
    AU_WaitCard,  //�ȴ�SD������
    AU_CheckApp, //���APP��
    AU_WaitJumpToApp,    //������app
} AU_Status_t;
static AU_Status_t procStatus = AU_PowerUp;

static uint64_t lastCheckTime = 0;
CL_INLINE void ProcStaToWaitCardStable()
{   //�л����ȿ��ȶ�״̬
    cardStableTime = 0;
    lastCheckTime = g_sysTotalTime;
    procStatus = AU_WaitCardStable;
}

void AppUpgrade_Process(void)
{
    uint64_t interval;
    static CL_BOOL waitForCard = CL_FALSE;
    switch (procStatus)
    {
        case AU_PowerUp:
            if(SD_CheckCard() == CL_TRUE)
            {   //��⵽�п�,�л����ȴ�sd���ȶ�״̬
                ProcStaToWaitCardStable();
                Log("�ϵ��⵽SD��,�ȴ��ȶ�\r\n");
            }
            else
            {   //û��sd��,�л������app��״̬
                procStatus = AU_CheckApp;
                Log("�ϵ�û��SD��,���app��\r\n");
            }
            break;
        case AU_WaitCardStable:
            interval = g_sysTotalTime - lastCheckTime;
            lastCheckTime = g_sysTotalTime;
            switch(CardStable(interval))
            {
                case 0: //�����γ���,�л���app���
                    procStatus = AU_CheckApp;
                    Log("SD�����γ�,���app��\r\n");
                    break;
                case 1: //���ȶ���,�л�������״̬
                    procStatus = AU_WriteToApp;
                    Log("SD���ȶ�,׼������\r\n");
                    break;
                case 2: //����û�ȶ�,������
                    //nothing to do
                    break;
            }
            
            break;
        case AU_WriteToApp:
            LED_SetEffect(LED_EFFECT_WORK, 0);
            if(Upgrade() == CL_SUCCESS)
            {   //д��ɹ�,�л������app��
                procStatus = AU_CheckApp;
                Log("�����ɹ�,���app��\r\n");
            }
            else
            {   //д��ʧ��,�л����ȴ�SD����״̬
                waitForCard = CL_FALSE;
                procStatus = AU_WaitCard;
                Log("����ʧ��,��ʼ���SD��\r\n");
            }
            LED_SetEffect(LED_EFFECT_IDLE, 0);
            break;
        case AU_WaitCard:
            if(SD_CheckCard() == CL_TRUE)   
            {   //��⵽�Ϳ�,�л����ȿ��ȶ���״̬
                if(waitForCard == CL_TRUE)
                {
                    ProcStaToWaitCardStable();
                    Log("��⵽SD��,�ȴ��ȶ�\r\n");
                }
            }
            else
            {   //�͵�ƽ�ſ�ʼ��⿨
                if(waitForCard == CL_FALSE)
                {
                    Log("�ȴ��忨\r\n");
                    waitForCard = CL_TRUE;
                }
            }
            break;
        case AU_CheckApp:
            if(IsAppValid())
            {   //app����Ч,������ת��־
                g_jumpToApp = CL_TRUE;
                procStatus = AU_WaitJumpToApp;
                Log("app�����ͨ��,��ת��app\r\n");
            }
            else
            {   //app��Ч,�л����ȴ�SD����״̬
                waitForCard = CL_FALSE;
                procStatus = AU_WaitCard;
                Log("app�����ʧ��,��ʼ���SD��\r\n");
            }
            break;
        case AU_WaitJumpToApp:
            //�ȴ���ת��app,ûʲô����
            
            break;
    }
}

//-----------------��ת��ָ����ַ---------------------------
typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.
static iapfun jump2app;

//����SP
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//��ת����,��һ��WORD���õ���ջ��ַ,�ڶ���WORD��ResetHandler,����������ջ,Ȼ����ת��ResetHandlerִ��
void JumpToAddr(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}	


