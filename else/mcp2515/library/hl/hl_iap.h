#pragma once

#include "cl_common.h"
#include "hl_config.h"

/******************************
 这些函数不要在中断里面调用

 *******************************/

//擦除和写操作前,需要先使能
extern int HL_IAP_Enable(uint32_t addr, uint32_t size);

//操作完之后,要关闭IAP
extern int HL_IAP_Disable(uint32_t addr, uint32_t size);

//从addr开始,擦除大小为size
extern int HL_IAP_Erase(uint32_t addr, uint32_t size);

//write之前,需要确保已经erase
extern int HL_IAP_Write(uint32_t addr, const uint8_t* buff, uint32_t length);


