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
static AppInfo_t appInfo; //app区的信息

//获取app区的hash
CL_INLINE static uint32_t GetAppHash()
{
    CRC_ResetDR();
    return CRC_CalcBlockCRC((uint32_t*)APP_START_ADDR, appInfo.size / 4);
}

//从flash读app信息
CL_INLINE static void ReadAppInfo()
{
    memcpy(&appInfo, (const void*)DAT_START_ADDR, sizeof(appInfo));
//    if(appInfo.size >= APP_SIZE)
//        appInfo.size = APP_SIZE;
}


//检测是否有有效的bin文件
CL_BOOL HasValidBinFile()
{
    FRESULT res;
    FIL file;
    uint8_t fileHead[8];
    UINT readBytes;
    DIR dir;
    FILINFO fno;
    
    //1.判断是否需要升级
    res = f_mount(&fs0, "0:/", 1);
    if(res != FR_OK)
        goto out;   //挂载失败

    res = f_opendir(&dir, IAP_BIN_FILE_DIR);
    if(res != FR_OK)
        goto out;   //打开目录失败

    //找到目标文件信息,检测文件大小
    for (;;) {
		res = f_readdir(&dir, &fno);                   /* Read a directory item */
		if (res != FR_OK || fno.fname[0] == 0) 
        {  //读取出错   
            res = FR_DISK_ERR;
			f_closedir(&dir);
            goto out;
        }
        
		if (fno.fattrib & AM_DIR) 
        {    
			continue;   //是个目录,跳过
		}
		else 
        {                                       /* It is a file. */
            if(strcasecmp(IAP_BIN_FILE_NAME, fno.fname) == 0)
            {   //文件名匹配,是目标文件
                if(fno.fsize > APP_SIZE)
                {   //文件过大,出错
                    res = FR_DISK_ERR;
			        f_closedir(&dir);
                    goto out;
                }
                else
                {   //文件大小合适,跳出循环
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
        goto out;  //打开文件失败

    //TODO: 文件大小检测
    //res = f_lseek(&file, 0);

    res = f_read(&file, fileHead, CL_ARRAY_LENGTH(fileHead), &readBytes); //read file;
    if(res != FR_OK)
        goto close_file; //读取文件失败

    if(readBytes != CL_ARRAY_LENGTH(fileHead)) 
    {   //读取长度错误
        res = FR_DISK_ERR;
        goto close_file;
    }

    if(fileHead[3] != 0x20 || fileHead[7] != 0x08)
    {   //文件头部数据错误
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

static uint8_t readBuff[READ_BYTES_ONCE]; //读文件的buff,硬编码为512了
//bin内容写入app区
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
        goto out;  //打开文件失败

    IWDG_Feed(); //擦除操作时间较长,先喂狗

    //擦除data区的app信息
    HL_IAP_Enable(DAT_START_ADDR, HL_IAP_PAGE_SIZE);
    HL_IAP_Erase(DAT_START_ADDR, HL_IAP_PAGE_SIZE);

    //擦除APP区
    HL_IAP_Enable(APP_START_ADDR, APP_SIZE); //开启iap
    HL_IAP_Erase(APP_START_ADDR, APP_SIZE); //擦除flash

    
    while(1)
    {
        res = f_read(&file, readBuff, CL_ARRAY_LENGTH(readBuff), &readBytes);
        if(res != FR_OK)
            goto disable_iap;  //读取失败

        if((readBytes % 4) != 0) //读取到的长度应该是4整数倍
        {
            res = FR_DISK_ERR;
            goto disable_iap;   
        }

        if(f_eof(&file))
        {   //到文件结尾了,可能没读满buff
            done = CL_TRUE;
        }
        else
        {   //没到文件结尾,应该读满了buff
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

        //读取没问题,写入到flash
        HL_IAP_Write(APP_START_ADDR + totalReadyBytes, readBuff, readBytes);
        appInfo.hash = CRC_CalcBlockCRC((uint32_t*)readBuff, readBytes / 4);
        
        totalReadyBytes += readBytes;

        IWDG_Feed(); //中间要喂狗,否则超时复位

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

    //把app信息存入数据区
    HL_IAP_Write(DAT_START_ADDR, (const uint8_t*)(&appInfo), sizeof(appInfo));
    
disable_iap:
    HL_IAP_Disable(APP_START_ADDR, APP_SIZE); //关闭iap
    HL_IAP_Disable(DAT_START_ADDR, HL_IAP_PAGE_SIZE);

    f_close(&file);
out:
    if(res != FR_OK)
        return CL_FAILED;
        
    return CL_SUCCESS;
}

//app区是否有效
CL_BOOL IsAppValid()
{
    uint32_t appHash;
    //app 头部检测
    uint32_t* pAppHead = (uint32_t*)APP_START_ADDR;
    if((pAppHead[0] & 0xff000000) != 0x20000000
        || (pAppHead[1] & 0xff000000) != 0x08000000)
        return CL_FALSE;

    //从数据区读出app信息
    ReadAppInfo();
    
    //如果app信息都为FFFF,则可能是出厂烧录,APP也判定为有效
    if(appInfo.hash == 0xffffffff &&
        appInfo.size == 0xffffffff)
        return CL_TRUE;
    
    //判断hash
    appHash = GetAppHash();
    if(appHash != appInfo.hash)
        return CL_FALSE;
    
    return CL_TRUE;
}




//初始化
void AppUpgrade_Init(void)
{
    //sd模块初始化
    SD_ModuleInit();

    //开crc时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

}

//退出
void AppUpgrade_Exit(void)
{
    SD_ModuleExit();
    //关crc时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, DISABLE);
}



static CL_RESULT Upgrade(void)
{
    if(HasValidBinFile() == CL_TRUE)   //检测是否有可用bin文件
    {   //有可用的bin文件
        if(FromBinToApp() == CL_SUCCESS)
        {   //把bin文件写入app区
            return CL_SUCCESS;
        }
        else
        {
            return CL_FAILED;
        }
    }
    else
    {
        Log("没有符合要求的文件\r\n");
        return CL_FAILED;
    }
        
}


static uint32_t cardStableTime = 0; //单位是ms
//返回1表示稳定,2表示还在等待稳定,0表示卡拔出
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
    AU_PowerUp = 0,   //上电
    AU_WaitCardStable,   //等待SD卡稳定
    AU_WriteToApp,   //写入app
    AU_WaitCard,  //等待SD卡插入
    AU_CheckApp, //检测APP区
    AU_WaitJumpToApp,    //等跳到app
} AU_Status_t;
static AU_Status_t procStatus = AU_PowerUp;

static uint64_t lastCheckTime = 0;
CL_INLINE void ProcStaToWaitCardStable()
{   //切换到等卡稳定状态
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
            {   //检测到有卡,切换到等待sd卡稳定状态
                ProcStaToWaitCardStable();
                Log("上电检测到SD卡,等待稳定\r\n");
            }
            else
            {   //没有sd卡,切换到检测app的状态
                procStatus = AU_CheckApp;
                Log("上电没有SD卡,检测app区\r\n");
            }
            break;
        case AU_WaitCardStable:
            interval = g_sysTotalTime - lastCheckTime;
            lastCheckTime = g_sysTotalTime;
            switch(CardStable(interval))
            {
                case 0: //卡被拔出了,切换到app检测
                    procStatus = AU_CheckApp;
                    Log("SD卡被拔出,检测app区\r\n");
                    break;
                case 1: //卡稳定了,切换到升级状态
                    procStatus = AU_WriteToApp;
                    Log("SD卡稳定,准备升级\r\n");
                    break;
                case 2: //卡还没稳定,继续等
                    //nothing to do
                    break;
            }
            
            break;
        case AU_WriteToApp:
            LED_SetEffect(LED_EFFECT_WORK, 0);
            if(Upgrade() == CL_SUCCESS)
            {   //写入成功,切换到检测app区
                procStatus = AU_CheckApp;
                Log("升级成功,检测app区\r\n");
            }
            else
            {   //写入失败,切换到等待SD卡的状态
                waitForCard = CL_FALSE;
                procStatus = AU_WaitCard;
                Log("升级失败,开始检测SD卡\r\n");
            }
            LED_SetEffect(LED_EFFECT_IDLE, 0);
            break;
        case AU_WaitCard:
            if(SD_CheckCard() == CL_TRUE)   
            {   //检测到油卡,切换到等卡稳定的状态
                if(waitForCard == CL_TRUE)
                {
                    ProcStaToWaitCardStable();
                    Log("检测到SD卡,等待稳定\r\n");
                }
            }
            else
            {   //低电平才开始检测卡
                if(waitForCard == CL_FALSE)
                {
                    Log("等待插卡\r\n");
                    waitForCard = CL_TRUE;
                }
            }
            break;
        case AU_CheckApp:
            if(IsAppValid())
            {   //app区有效,设置跳转标志
                g_jumpToApp = CL_TRUE;
                procStatus = AU_WaitJumpToApp;
                Log("app区检测通过,跳转到app\r\n");
            }
            else
            {   //app无效,切换到等待SD卡的状态
                waitForCard = CL_FALSE;
                procStatus = AU_WaitCard;
                Log("app区检测失败,开始检测SD卡\r\n");
            }
            break;
        case AU_WaitJumpToApp:
            //等待跳转到app,没什么事做
            
            break;
    }
}

//-----------------跳转到指定地址---------------------------
typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
static iapfun jump2app;

//设置SP
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//跳转函数,第一个WORD放置的是栈地址,第二个WORD是ResetHandler,所以先设置栈,然后跳转到ResetHandler执行
void JumpToAddr(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}	


