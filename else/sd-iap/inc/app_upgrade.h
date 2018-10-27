#pragma once
#include "cl_common.h"

typedef struct
{
    uint32_t size;
    uint32_t hash;
} AppInfo_t;

#define IAP_BIN_FILE_DIR    "0:/"       //�ļ�����Ŀ¼
#define IAP_BIN_FILE_NAME   "dd.bin"    //�ļ���
#define IAP_BIN_FILE_PATH   "0:/dd.bin" //�ļ�����·��

extern void AppUpgrade_Init(void);
extern void AppUpgrade_Exit(void);
extern void AppUpgrade_Process(void);


extern void JumpToAddr(uint32_t appxaddr);


