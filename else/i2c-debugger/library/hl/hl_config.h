/**
  ******************************************************************************
  * @file    HL_Config.h
  * @author  LYJ
  * @version V3.5.0
  * @brief   HL库的配置文件,这个库依赖CLIB库
  ******************************************************************************
  */


#pragma once

enum
{
    HL_STM32F10X_CL = 0,
};

#define HL_MCU_TYPE HL_STM32F10X_CL


//flash page size
#if HL_MCU_TYPE == HL_STM32F10X_CL
#define HL_IAP_PAGE_SIZE  (1024 * 2)  //2K
#endif

