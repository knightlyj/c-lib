#include "cl_queue.h"
#include "cl_atomic.h"

#define DATA_CPY(DST, SRC, SIZE) memcpy(DST, SRC, SIZE)

CL_RESULT CL_QueueAdd(CL_QueueInfo_t *q, void *data)
{
    if (CL_QueueFull(q))
        return CL_FAILED;

    DATA_CPY((char *)q->data + (q->tail * q->data_size), data, q->data_size);
    q->tail += 1;
    if (q->tail >= q->capacity)
    {
        q->tail = 0;
    }

#if CL_CPU_TYPE == CL_CORTEX_M3 || CL_CPU_TYPE == CL_CORTEX_M4
    atomic_add(&q->length, 1);
#elif CL_CPU_TYPE == CL_X64
    (q->length)++;
#endif

    assert(q->head >= 0 && q->head < q->capacity);
    assert(q->tail >= 0 && q->tail < q->capacity);
    assert(q->length >= 0 && q->length <= q->capacity);


    return CL_SUCCESS;
}

CL_RESULT CL_QueuePoll(CL_QueueInfo_t *q, void *data)
{
    if (CL_QueueEmpty(q))
        return CL_FAILED;
    DATA_CPY(data, (char *)q->data + (q->head * q->data_size), q->data_size);
    q->head += 1;
    if (q->head >= q->capacity)
    {
        q->head = 0;
    }

#if CL_CPU_TYPE == CL_CORTEX_M3 || CL_CPU_TYPE == CL_CORTEX_M4
    atomic_add(&q->length, -1);
#elif CL_CPU_TYPE == CL_X64
    (q->length)--;
#endif

    assert(q->head >= 0 && q->head < q->capacity);
    assert(q->tail >= 0 && q->tail < q->capacity);
    assert(q->length >= 0 && q->length <= q->capacity);


    return CL_SUCCESS;
}

//查看队列头数据
CL_RESULT CL_QueuePeek(CL_QueueInfo_t *q, void *data)
{
    if (CL_QueueEmpty(q))
        return CL_FAILED;

    DATA_CPY(data, (char *)q->data + (q->head * q->data_size), q->data_size);

    return CL_SUCCESS;
}

//一次Add一大段数据,传入的length是实际入队元素个数,不是以byte记的
//CL_RESULT CL_QueueAddEx(CL_QueueInfo_t *q, void* buff, int addCount)
//{
//    if (q->capacity - q->length < addCount)    //空间不足
//        return CL_FAILED;
//
//    int tailToEnd = q->capacity - q->tail;  //从tail到缓冲区结束位置的空间
//    if (tailToEnd >= addCount)
//    {   //从tail到缓冲区结束位置的空间足够,一次copy完就行了
//        DATA_CPY((char *)q->data + (q->tail * q->data_size), buff, q->data_size * addCount);
//
//        q->tail += addCount;
//    }
//    else
//    {   //从tail到缓冲区结束的空间不够,先copy到缓冲区结束
//        DATA_CPY((char *)q->data + (q->tail * q->data_size), buff, q->data_size * tailToEnd);
//        //再从缓冲区起始位置copy
//        DATA_CPY((char *)q->data, (char *)buff + (tailToEnd * q->data_size), q->data_size * (addCount - tailToEnd));
//
//        q->tail = addCount - tailToEnd;
//    }
//    
//
//#if CL_CPU_TYPE == CL_CORTEX_M3 || CL_CPU_TYPE == CL_CORTEX_M4
//    atomic_add(&q->length, count);
//#elif CL_CPU_TYPE == CL_X64
//    (q->length) += addCount;
//#endif
//
//    return CL_SUCCESS;
//}

