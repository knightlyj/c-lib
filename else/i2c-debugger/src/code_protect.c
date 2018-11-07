#include "cl_common.h"
#include "stm32f10x_flash.h"

#include "flash_layout.h"

#define kLOCKCODEADDRESS    (FLASH_START_ADDR + 1024 * 4)                                 // 这个 32 位数据，是数据加密的 32 位
const uint32_t Test_Addr __attribute__((at(kLOCKCODEADDRESS)))=0xFFFFFFFF;    // 在该地址写入0xFFFFFFFF
u32 CpuID[3];
u32 Lock_Code = 0xffffffff;

/*
 **************************************************************
 * 得到加密的数据值
 **************************************************************
 */
void GetLockCode(void)
{
    // 获取CPU唯一ID
    CpuID[0] = *(vu32*)(0x1ffff7e8);
    CpuID[1] = *(vu32*)(0x1ffff7ec);
    CpuID[2] = *(vu32*)(0x1ffff7f0);
    //加密算法,很简单的加密算法
    Lock_Code = (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}

/*
 *************************************************************** 
 * 获取 STM32 加密区的数据值
 *************************************************************** 
 */
u32 GetLockCodeFromFlash(void)
{
    //  u32 code1 = (u32)(*(u32*)(kLOCKCODEADDRESS+2))<<16;
    
    //  u32 code2 = (u32)(*(u32*)(kLOCKCODEADDRESS))&0x0000FFFF;
    
    //  DLog("code1:[%x], code2:[%x]\n", code1, code2);
    
    return ((u32)(*(u32*)(kLOCKCODEADDRESS+2))<<16) + ((u32)(*(u32*)(kLOCKCODEADDRESS))&0x0000FFFF);
}

/*
 ***************************************************************
 * 保存加密的数据
 ***************************************************************
 */
void SaveLockCode(void)
{
    u32 addressTmp = kLOCKCODEADDRESS;
    
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
    //FLASH_ErasePage(addressTmp);
    
    FLASH_ProgramHalfWord(addressTmp, (u16)(Lock_Code&0xffff));
    
    addressTmp+=2;
    
    FLASH_ProgramHalfWord(addressTmp,(u16)(Lock_Code>>16));
            
    FLASH_Lock();
        
//  *(u32*)addressTmp = (u16)(Lock_Code&0xffff);
//  
//  addressTmp+=2;
//  
//  *(u32*)addressTmp = (u16)(Lock_Code>>16);
}

uint8_t check(void)
{
    if(Lock_Code == 0xffffffff) GetLockCode();
    
    if(GetLockCodeFromFlash() == 0xffffffff)        // 芯片是空白的，就往内面写入 经过计算以后的加密数据
    {
        SaveLockCode();
    }
        
//    DLog("Lock_Code:[%x], flash_code:[%x]\n", Lock_Code, GetLockCodeFromFlash());
        
    if(GetLockCodeFromFlash() != Lock_Code)
    {
//        DLog("Check Error, Lock Code Error\n");
        return CL_FALSE;
    }
    else
    {
//        DLog("Check Ok\n");
        return  CL_TRUE;
    }
}
