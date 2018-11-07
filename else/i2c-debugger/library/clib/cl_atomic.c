#include "cl_common.h"

#if CL_CPU_TYPE == CL_CORTEX_M3
//KEIL的asm函数语法
//实现原子自增
__asm void atomic_add(int32_t *i, int32_t n)
{
try_add
    LDREX r2, [r0]
    ADD   r2, r2, r1
    STREX r3, r2, [r0]
    CMP r3, #0
    BNE try_add 

    BX    lr
}
#endif

