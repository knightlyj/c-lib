#pragma once
#include "stdint.h"

/*FLASH布局如下
 [IAP][APP][DAT]
 */
#define FLASH_START_ADDR    ((uint32_t)0x08000000)       //FLASH起始地址
#define FLASH_TOTAL_SIZE    ((uint32_t)(1024 * 128))    //FLASH总尺寸

#define IAP_SIZE     ((uint32_t)(1024 * 22))    //IAP为22k
#define APP_SIZE     ((uint32_t)(1024 * 100))    //APP区域为100k
#define DAT_SIZE     ((uint32_t)(FLASH_TOTAL_SIZE - IAP_SIZE - APP_SIZE))    //数据区尺寸

#define IAP_START_ADDR      (FLASH_START_ADDR)              //IAP区起始地址
#define APP_START_ADDR      (IAP_START_ADDR + IAP_SIZE)     //APP区起始地址
#define DAT_START_ADDR      (APP_START_ADDR + APP_SIZE)     //DAT区起始地址

/*-------------------------------
 *数据区使用
 PAGE0: 
    iap使用
-------------------------------*/


