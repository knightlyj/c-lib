/*********************************************************************************
*Description:  这是一个泛型环形队列,队列容量必须为const(编译时确定)
**********************************************************************************/

#pragma once

#include "common.h"

/**
* @brief 队列所用数据结构
*/
typedef struct{ 
    void *data; 
    unsigned int head;
    unsigned int tail;
    unsigned int length;
    unsigned int capacity;
    unsigned int data_size;
} queue_type;

/** 
 * @brief 定义并初始化队列
 * @param q_name 队列命
 * @param capacity 队列容量
 * @param data_type 队列数据类型
 * @param modifier 附加的修饰，如static等
 *
 * example: 
 *      QUEUE_DEF_INIT(my_queue, 10, char, static);
 *      以上定义了一个static的队列，队列名为my_queue, 容量为10，队列中的数据类型为char
 */
#define QUEUE_DEF_INIT(q_name, capacity, data_type, modifier) \
modifier data_type q_name##_buf[capacity + 1]; \
modifier queue_type q_name = {q_name##_buf, 0, 0, 0, capacity, sizeof(data_type)};

/** 
 * @brief 同上,但是用来声明
 */
#define QUEUE_DECL(q_name) \
extern queue_type q_name;

/** 
 * @brief 向队列增加一个数据
 * @param q 队列指针
 * @param data 需要加入队列的数据的指针
 *
 * @return
 *     FAILED 操作失败,一般是队列满了
 *     SUCCESS 操作成功
 */
int queue_add(queue_type *q, const void *data);

/** 
 * @brief 从队列头部获取一个数据,并将这个数据从队列移除
 * @param q 队列指针
 * @param data 指针,指向获取数据存放地址
 *
 * @return 
 *     FAILED 操作失败,一般是队列为空
 *     SUCCESS 操作成功
 */
int queue_poll(queue_type *q, void *data);

/** 
 * @brief 获取队列头部数据,但不从队列移除此数据
 * @param q 队列指针
 * @param data 指针,指向获取数据存放地址
 *
 * @return 
 *     FAILED 操作失败,一般是队列为空
 *     SUCCESS 操作成功
 */
int queue_peek(queue_type *q, void *data);

/** 
 * @brief 获取队列当前长度
 * @param q 队列指针
 *
 * @return 
 *    返回队列当前长度
 */
INLINE static unsigned int queue_length(queue_type *q){
    return q->length;
}

/** 
 * @brief 清空队列
 * @param q 队列指针
 *
 */
INLINE static void queue_clear(queue_type *q){
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
INLINE static unsigned int queue_capacity(queue_type *q){
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
INLINE static int queue_is_empty(queue_type *q){
    return q->length == 0;
}

/** 
 * @brief 队列是否满
 * @param q 队列指针
 *
 * @return 
 *    TRUE 队列满
 *    FALSE 队列没满
 */
INLINE static int queue_is_full(queue_type *q){
    return q->length >= q->capacity;
}

/** 
 * @brief 迭代访问队列中的每个元素
 * @param q_ptr 队列指针
 * @param data_ptr 指向数据地址的指针
 * @param data_type 数据的类型
 * 
 * example: 
 *      QUEUE_DEF_INIT(my_queue, 10, char, static);
 *      //以上定义了一个static的队列，队列名为my_queue, 容量为10，队列中的数据类型为char
 *      char* data;
 *      QUEUE_FOR_EACH(my_queue, data, char)    //迭代访问队列中每个数据
 *      {
 *          printf("%c", *data);    //把这个数据打印出来
 *      }
 */
#define QUEUE_FOR_EACH(q_ptr, data_ptr, data_type) \
	for(data_ptr = ((data_type *)((q_ptr)->data)) + (q_ptr)->head; \
		data_ptr != ((data_type *)((q_ptr)->data)) + (q_ptr)->tail; \
		(++data_ptr) >= ((data_type *)((q_ptr)->data) + (q_ptr)->capacity) ? data_ptr = (data_type *)((q_ptr)->data) : data_ptr)

