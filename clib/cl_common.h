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
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CL_CORTEX_M3    (3)
#define CL_CORTEX_M4    (4)
#define CL_X64          (64)

#define CL_CPU_TYPE     CL_X64

typedef enum
{
    CL_FALSE = 0,
    CL_TRUE = 1,
} CL_BOOL;

typedef enum
{
    CL_SUCCESS = 0,

    CL_FAILED = -1,
    CL_INVALID_PARAM = -2,

} CL_RESULT;


#define CL_INLINE           __inline
#define CL_STATIC_INLINE    static CL_INLINE
#define CL_NULL             (0)

#ifdef _DEBUG
#include "assert.h"
#else
#define assert(expression)
#endif

#define CL_ARRAY_LENGTH(array)      (sizeof(array) / sizeof(array[0]))

#define Log     printf

#ifdef __cplusplus
}
#endif


