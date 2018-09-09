/*********************************************************************************
*Description:  ����һ�����ͻ��ζ���,������������Ϊconst(����ʱȷ��)
**********************************************************************************/

#pragma once

#include "common.h"

/**
* @brief �����������ݽṹ
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
 * @brief ���岢��ʼ������
 * @param q_name ������
 * @param capacity ��������
 * @param data_type ������������
 * @param modifier ���ӵ����Σ���static��
 *
 * example: 
 *      QUEUE_DEF_INIT(my_queue, 10, char, static);
 *      ���϶�����һ��static�Ķ��У�������Ϊmy_queue, ����Ϊ10�������е���������Ϊchar
 */
#define QUEUE_DEF_INIT(q_name, capacity, data_type, modifier) \
modifier data_type q_name##_buf[capacity + 1]; \
modifier queue_type q_name = {q_name##_buf, 0, 0, 0, capacity, sizeof(data_type)};

/** 
 * @brief ͬ��,������������
 */
#define QUEUE_DECL(q_name) \
extern queue_type q_name;

/** 
 * @brief ���������һ������
 * @param q ����ָ��
 * @param data ��Ҫ������е����ݵ�ָ��
 *
 * @return
 *     FAILED ����ʧ��,һ���Ƕ�������
 *     SUCCESS �����ɹ�
 */
int queue_add(queue_type *q, const void *data);

/** 
 * @brief �Ӷ���ͷ����ȡһ������,����������ݴӶ����Ƴ�
 * @param q ����ָ��
 * @param data ָ��,ָ���ȡ���ݴ�ŵ�ַ
 *
 * @return 
 *     FAILED ����ʧ��,һ���Ƕ���Ϊ��
 *     SUCCESS �����ɹ�
 */
int queue_poll(queue_type *q, void *data);

/** 
 * @brief ��ȡ����ͷ������,�����Ӷ����Ƴ�������
 * @param q ����ָ��
 * @param data ָ��,ָ���ȡ���ݴ�ŵ�ַ
 *
 * @return 
 *     FAILED ����ʧ��,һ���Ƕ���Ϊ��
 *     SUCCESS �����ɹ�
 */
int queue_peek(queue_type *q, void *data);

/** 
 * @brief ��ȡ���е�ǰ����
 * @param q ����ָ��
 *
 * @return 
 *    ���ض��е�ǰ����
 */
INLINE static unsigned int queue_length(queue_type *q){
    return q->length;
}

/** 
 * @brief ��ն���
 * @param q ����ָ��
 *
 */
INLINE static void queue_clear(queue_type *q){
    q->head = q->tail = 0;
    q->length = 0;
}

/** 
 * @brief ��ȡ��������
 * @param q ����ָ��
 *
 * @return 
 *    ���ض��ж�������
 */
INLINE static unsigned int queue_capacity(queue_type *q){
    return q->capacity;
}

/** 
 * @brief �����Ƿ��
 * @param q ����ָ��
 *
 * @return 
 *    TRUE ����Ϊ��
 *    FALSE ���зǿ�
 */
INLINE static int queue_is_empty(queue_type *q){
    return q->length == 0;
}

/** 
 * @brief �����Ƿ���
 * @param q ����ָ��
 *
 * @return 
 *    TRUE ������
 *    FALSE ����û��
 */
INLINE static int queue_is_full(queue_type *q){
    return q->length >= q->capacity;
}

/** 
 * @brief �������ʶ����е�ÿ��Ԫ��
 * @param q_ptr ����ָ��
 * @param data_ptr ָ�����ݵ�ַ��ָ��
 * @param data_type ���ݵ�����
 * 
 * example: 
 *      QUEUE_DEF_INIT(my_queue, 10, char, static);
 *      //���϶�����һ��static�Ķ��У�������Ϊmy_queue, ����Ϊ10�������е���������Ϊchar
 *      char* data;
 *      QUEUE_FOR_EACH(my_queue, data, char)    //�������ʶ�����ÿ������
 *      {
 *          printf("%c", *data);    //��������ݴ�ӡ����
 *      }
 */
#define QUEUE_FOR_EACH(q_ptr, data_ptr, data_type) \
	for(data_ptr = ((data_type *)((q_ptr)->data)) + (q_ptr)->head; \
		data_ptr != ((data_type *)((q_ptr)->data)) + (q_ptr)->tail; \
		(++data_ptr) >= ((data_type *)((q_ptr)->data) + (q_ptr)->capacity) ? data_ptr = (data_type *)((q_ptr)->data) : data_ptr)

