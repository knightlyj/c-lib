#pragma once
#include "stdint.h"

/*FLASH��������
 [IAP][APP][DAT]
 */
#define FLASH_START_ADDR    ((uint32_t)0x08000000)       //FLASH��ʼ��ַ
#define FLASH_TOTAL_SIZE    ((uint32_t)(1024 * 128))    //FLASH�ܳߴ�

#define IAP_SIZE     ((uint32_t)(1024 * 22))    //IAPΪ22k
#define APP_SIZE     ((uint32_t)(1024 * 100))    //APP����Ϊ100k
#define DAT_SIZE     ((uint32_t)(FLASH_TOTAL_SIZE - IAP_SIZE - APP_SIZE))    //�������ߴ�

#define IAP_START_ADDR      (FLASH_START_ADDR)              //IAP����ʼ��ַ
#define APP_START_ADDR      (IAP_START_ADDR + IAP_SIZE)     //APP����ʼ��ַ
#define DAT_START_ADDR      (APP_START_ADDR + APP_SIZE)     //DAT����ʼ��ַ

/*-------------------------------
 *������ʹ��
 PAGE0: 
    iapʹ��
-------------------------------*/


