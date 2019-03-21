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

//�鿴����ͷ����
CL_RESULT CL_QueuePeek(CL_QueueInfo_t *q, void *data)
{
    if (CL_QueueEmpty(q))
        return CL_FAILED;

    DATA_CPY(data, (char *)q->data + (q->head * q->data_size), q->data_size);

    return CL_SUCCESS;
}

//һ��Addһ�������,�����length��ʵ�����Ԫ�ظ���,������byte�ǵ�
//CL_RESULT CL_QueueAddEx(CL_QueueInfo_t *q, void* buff, int addCount)
//{
//    if (q->capacity - q->length < addCount)    //�ռ䲻��
//        return CL_FAILED;
//
//    int tailToEnd = q->capacity - q->tail;  //��tail������������λ�õĿռ�
//    if (tailToEnd >= addCount)
//    {   //��tail������������λ�õĿռ��㹻,һ��copy�������
//        DATA_CPY((char *)q->data + (q->tail * q->data_size), buff, q->data_size * addCount);
//
//        q->tail += addCount;
//    }
//    else
//    {   //��tail�������������Ŀռ䲻��,��copy������������
//        DATA_CPY((char *)q->data + (q->tail * q->data_size), buff, q->data_size * tailToEnd);
//        //�ٴӻ�������ʼλ��copy
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

