#pragma once

#include "stdint.h"
#include "string.h"

#define NOTHING

#define TRUE        (1)
#define FALSE       (0)

#define SUCCESS     (0)
#define FAILED      (-1)

#define INLINE      inline

#ifdef _DEBUG
#include "assert.h"
#else
#define assert(expression)      TRUE
#endif




