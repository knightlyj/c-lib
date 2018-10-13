#pragma once

#include "stdint.h"
#include "string.h"

enum
{
    CL_CORTEX_M3 = 3,
};

#define CL_CPU_TYPE CL_CORTEX_M3


#define CL_NOTHING

#define CL_TRUE             (1)
#define CL_FALSE            (0)

#define CL_SUCCESS          (0)
#define CL_FAILED           (-1)
#define CL_INVALID_PARAM    (-2)

#define CL_INLINE           __inline

#ifdef _DEBUG
#include "assert.h"
#else
#define assert(expression)      CL_NOTHING
#endif



