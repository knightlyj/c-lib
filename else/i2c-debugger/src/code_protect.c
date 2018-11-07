#include "cl_common.h"
#include "stm32f10x_flash.h"

#include "flash_layout.h"

#define kLOCKCODEADDRESS    (FLASH_START_ADDR + 1024 * 4)                                 // ��� 32 λ���ݣ������ݼ��ܵ� 32 λ
const uint32_t Test_Addr __attribute__((at(kLOCKCODEADDRESS)))=0xFFFFFFFF;    // �ڸõ�ַд��0xFFFFFFFF
u32 CpuID[3];
u32 Lock_Code = 0xffffffff;

/*
 **************************************************************
 * �õ����ܵ�����ֵ
 **************************************************************
 */
void GetLockCode(void)
{
    // ��ȡCPUΨһID
    CpuID[0] = *(vu32*)(0x1ffff7e8);
    CpuID[1] = *(vu32*)(0x1ffff7ec);
    CpuID[2] = *(vu32*)(0x1ffff7f0);
    //�����㷨,�ܼ򵥵ļ����㷨
    Lock_Code = (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}

/*
 *************************************************************** 
 * ��ȡ STM32 ������������ֵ
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
 * ������ܵ�����
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
    
    if(GetLockCodeFromFlash() == 0xffffffff)        // оƬ�ǿհ׵ģ���������д�� ���������Ժ�ļ�������
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
