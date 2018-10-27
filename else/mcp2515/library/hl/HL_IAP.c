#include "hl_iap.h"

#include "stdio.h"

#if HL_MCU_TYPE == HL_STM32F10X_CL
#include "platform/stm32/__hl_iap_stm32.h"

#endif


int HL_IAP_Enable(uint32_t addr, uint32_t size)
{
    //地址应该对齐于page
    if((addr % HL_IAP_PAGE_SIZE) != 0)
        return CL_INVALID_PARAM;

    if(size == 0 || (size % HL_IAP_PAGE_SIZE) != 0)
        return CL_INVALID_PARAM;

    return __HL_IAP_Enable(addr, size);
}


int HL_IAP_Disable(uint32_t addr, uint32_t size)
{
    //地址应该对齐于page
    if((addr % HL_IAP_PAGE_SIZE) != 0)
        return CL_INVALID_PARAM;

    if(size == 0 || (size % HL_IAP_PAGE_SIZE) != 0)
        return CL_INVALID_PARAM;

    return __HL_IAP_Disable(addr, size);
}

int HL_IAP_Erase(uint32_t addr, uint32_t size)
{
    //地址应该对齐于page
    if((addr % HL_IAP_PAGE_SIZE) != 0)
        return CL_INVALID_PARAM;

    if(size == 0 || (size % HL_IAP_PAGE_SIZE) != 0)
        return CL_INVALID_PARAM;

    return __HL_IAP_Erase(addr, size);
}


int HL_IAP_Write(uint32_t addr, const uint8_t* buff, uint32_t length)
{

    return __HL_IAP_WritePage(addr, buff, length);
}





