#include "queue.h"
#include <iostream>

#define DATA_CPY(DST, SRC, SIZE) memcpy(DST, SRC, SIZE)

int queue_add(queue_type *q, const void *data)
{
    if (queue_is_full(q))
        return FAILED;
    DATA_CPY((char *)q->data + (q->tail * q->data_size), data, q->data_size);
    q->tail += 1;
    if (q->tail >= q->capacity) {
        q->tail = 0;
    }
    q->length += 1;

    assert(q->head >= 0 && q->head < q->capacity);
    assert(q->tail >= 0 && q->tail < q->capacity);
    assert(q->length >= 0 && q->length <= q->capacity);

    return SUCCESS;
}

int queue_poll(queue_type *q, void *data)
{
    if (queue_is_empty(q))
        return FAILED;
    DATA_CPY(data, (char *)q->data + (q->head * q->data_size), q->data_size);
    q->head += 1;
    if (q->head >= q->capacity) {
        q->head = 0;
    }
    q->length -= 1;

    assert(q->head >= 0 && q->head < q->capacity);
    assert(q->tail >= 0 && q->tail < q->capacity);
    assert(q->length >= 0 && q->length <= q->capacity);

    return SUCCESS;
}

int queue_peek(queue_type *q, void *data)
{
    if (queue_is_empty(q))
        return FAILED;

    DATA_CPY(data, (char *)q->data + (q->head * q->data_size), q->data_size);
    
    return SUCCESS;
}
