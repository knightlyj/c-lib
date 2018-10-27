#include "stm32f10x_flash.h"
static int __HL_IAP_WritePage(uint32_t addr, const uint8_t* buff, uint32_t length)
{
    uint32_t writeAddr, offset;
    uint32_t data;
    FLASH_Status status;


    /* Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    //little endian [0][1][2][3] -> 3210
    //stm32f10x是小端模式,这里按小端模式组装成32位,写入后与buff的字节序相同
    offset = 0;
    writeAddr = addr;
    while(offset < length)
    {
        data = buff[offset++];
        if(offset < length)
            data |= buff[offset++] << 8;
        if(offset < length)
            data |= buff[offset++] << 16;
        if(offset < length)
            data |= buff[offset++] << 24;

        status = FLASH_ProgramWord(writeAddr, data);
        if(status != FLASH_COMPLETE)
            return CL_FAILED;

        writeAddr += 4;
    }
    return CL_SUCCESS;
}


static int flashUnlockCounter = 0; //flash unlock 计数
static int __HL_IAP_Enable(uint32_t addr, uint32_t size)
{
    //这里没有考虑写保护
    flashUnlockCounter++;
    FLASH_Unlock();
    return CL_SUCCESS;
}

static int __HL_IAP_Disable(uint32_t addr, uint32_t size)
{
    uint32_t lockStatus;
    //这里没有考虑写保护
    flashUnlockCounter--;
    if(flashUnlockCounter <= 0)
    {
        FLASH_Lock();
    }

    lockStatus = FLASH->CR & 0x80;
    if(lockStatus == 0x80)
        Log("flash locked\r\n");

    return CL_SUCCESS;
}


static int __HL_IAP_Erase(uint32_t addr, uint32_t size)
{
    uint32_t eraseSize = 0;
    FLASH_Status status;

    /* Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    while(eraseSize < size)
    {
        status = FLASH_ErasePage(addr + eraseSize);
        if(status != FLASH_COMPLETE)
            return CL_FAILED;

        eraseSize += HL_IAP_PAGE_SIZE;
    }

    return CL_SUCCESS;
}

