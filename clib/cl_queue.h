/*********************************************************************************
*Description:  这是一个泛型环形队列,队列容量必须为const(编译时确定)
*Warnings:     add和poll功能可以同时进行,但单独add或poll功能不可重入
**********************************************************************************/

#pragma once

#include "cl_common.h"
#if CL_CPU_TYPE == CL_X64
#include <atomic>   //原子操作库
#endif

#ifdef __cplusplus
extern "C" {
#endif
   
/**
* @brief 队列所用数据结构
*/
typedef struct{ 
    volatile void *data;
    volatile int32_t head;
    volatile int32_t tail;
#if CL_CPU_TYPE == CL_CORTEX_M3 || CL_CPU_TYPE == CL_CORTEX_M4
    volatile int32_t length;
#elif   CL_CPU_TYPE == CL_X64
    std::atomic<int> length;
#endif
    volatile int32_t capacity;
    volatile int32_t data_size;
} CL_QueueInfo_t;

/** 
 * @brief 定义并初始化队列
 * @param q_name 队列命
 * @param capacity 队列容量
 * @param data_type 队列数据类型
 * @param modifier 附加的修饰，如static等
 *
 * example: 
 *      CL_QUEUE_DEF_INIT(my_queue, 10, char, static);
 *      以上定义了一个static的队列，队列名为my_queue, 容量为10，队列中的数据类型为char
 */
#define CL_QUEUE_DEF_INIT(q_name, capacity, data_type, modifier) \
static data_type q_name##_buf[capacity]; \
modifier CL_QueueInfo_t q_name = {q_name##_buf, 0, 0, 0, capacity, sizeof(data_type)};

/** 
 * @brief 同上,但是用来声明
 */
#define CL_QUEUE_DECL(q_name) \
extern CL_QueueInfo_t q_name;

/** 
 * @brief 向队列增加一个数据
 * @param q 队列指针
 * @param data 需要加入队列的数据的指针
 *
 * @return
 *     FAILED 操作失败,一般是队列满了
 *     SUCCESS 操作成功
 */
CL_RESULT CL_QueueAdd(CL_QueueInfo_t *q, void *data);

/** 
 * @brief 从队列头部获取一个数据,并将这个数据从队列移除
 * @param q 队列指针
 * @param data 指针,指向获取数据存放地址
 *
 * @return 
 *     FAILED 操作失败,一般是队列为空
 *     SUCCESS 操作成功
 */
CL_RESULT CL_QueuePoll(CL_QueueInfo_t *q, void *data);

/** 
 * @brief 获取队列头部数据,但不从队列移除此数据
 * @param q 队列指针
 * @param data 指针,指向获取数据存放地址
 *
 * @return 
 *     FAILED 操作失败,一般是队列为空
 *     SUCCESS 操作成功
 */
CL_RESULT CL_QueuePeek(CL_QueueInfo_t *q, void *data);

/** 
 * @brief 获取队列当前长度
 * @param q 队列指针
 *
 * @return 
 *    返回队列当前长度
 */
CL_STATIC_INLINE int32_t CL_QueueLength(CL_QueueInfo_t *q){
    return q->length;
}

/** 
 * @brief 清空队列
 * @param q 队列指针
 *
 */
CL_STATIC_INLINE void CL_QueueClear(CL_QueueInfo_t *q){
    q->head = q->tail = 0;
    q->length = 0;
}

/** 
 * @brief 获取队列容量
 * @param q 队列指针
 *
 * @return 
 *    返回队列队列容量
 */
CL_STATIC_INLINE int32_t CL_QueueCapacity(CL_QueueInfo_t *q){
    return q->capacity;
}

/** 
 * @brief 队列是否空
 * @param q 队列指针
 *
 * @return 
 *    TRUE 队列为空
 *    FALSE 队列非空
 */
CL_STATIC_INLINE CL_BOOL CL_QueueEmpty(CL_QueueInfo_t *q){
    return (q->length <= 0) ? CL_TRUE : CL_FALSE;
}

/** 
 * @brief 队列是否满
 * @param q 队列指针
 *
 * @return 
 *    TRUE 队列满
 *    FALSE 队列没满
 */
CL_STATIC_INLINE CL_BOOL CL_QueueFull(CL_QueueInfo_t *q){
    return (q->length >= q->capacity) ? CL_TRUE : CL_FALSE;
}

/** 
 * @brief 迭代访问队列中的每个元素
 * @param q_ptr 队列指针
 * @param data_ptr 指向数据地址的指针
 * @param data_type 数据的类型
 * 
 * example: 
 *      CL_QUEUE_DEF_INIT(my_queue, 10, char, static);
 *      //以上定义了一个static的队列，队列名为my_queue, 容量为10，队列中的数据类型为char
 *      char* data;
 *      CL_QUEUE_FOR_EACH(my_queue, data, char)    //迭代访问队列中每个数据
 *      {
 *          printf("%c", *data);    //把这个数据打印出来               
 *      }
 */
#define CL_QUEUE_FOR_EACH(q_ptr, data_ptr, data_type) \
    data_ptr = ((data_type *)((q_ptr)->data)) + (q_ptr)->head; \
	for(int ___q_itrcnt___ = (q_ptr)->length; \
		___q_itrcnt___ != 0; \
		___q_itrcnt___--, (++data_ptr) >= ((data_type *)((q_ptr)->data) + (q_ptr)->capacity) ? data_ptr = (data_type *)((q_ptr)->data) : data_ptr)


#ifdef __cplusplus
}
#endif


