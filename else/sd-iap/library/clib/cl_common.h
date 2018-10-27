/**
  ******************************************************************************
  * @file    CL_Common.h
  * @author  LYJ
  * @brief   CLIB的目的是编写跨平台的C库,为了支持原子操作,需要定义CPU的类型
  ******************************************************************************
  */


#pragma once

#include "stdint.h"
#include "string.h"

enum
{
    CL_CORTEX_M3 = 3,
};

#define CL_CPU_TYPE CL_CORTEX_M3


#define CL_NOTHING


#define CL_BOOL             uint8_t
#define CL_TRUE             (1)
#define CL_FALSE            (0)


#define CL_RESULT           int32_t
#define CL_SUCCESS          (0)
#define CL_FAILED           (-1)
#define CL_INVALID_PARAM    (-2)

#define CL_INLINE           __inline

#ifdef _DEBUG
#include "assert.h"
#else
#define assert(expression)      CL_NOTHING
#endif

#define CL_ARRAY_LENGTH(array)      (sizeof(array) / sizeof(array[0]))

#define Log     printf


