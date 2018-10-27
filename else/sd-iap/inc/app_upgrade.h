#pragma once
#include "cl_common.h"

typedef struct
{
    uint32_t size;
    uint32_t hash;
} AppInfo_t;

#define IAP_BIN_FILE_DIR    "0:/"       //文件所在目录
#define IAP_BIN_FILE_NAME   "dd.bin"    //文件名
#define IAP_BIN_FILE_PATH   "0:/dd.bin" //文件完整路径

extern void AppUpgrade_Init(void);
extern void AppUpgrade_Exit(void);
extern void AppUpgrade_Process(void);


extern void JumpToAddr(uint32_t appxaddr);


