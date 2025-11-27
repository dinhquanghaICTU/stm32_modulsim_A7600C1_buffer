#include "services/event_queue.h"
#include <string.h>

void event_queue_init(event_queue_t *q)
{
    memset(q, 0, sizeof(*q));
}

bool event_queue_is_empty(event_queue_t *q)
{
    return q->count == 0;
}

bool event_queue_is_full(event_queue_t *q)
{
    return q->count >= EVENT_QUEUE_SIZE;
}

bool event_queue_push(event_queue_t *q, const at_event_t *evt)
{
    if (event_queue_is_full(q))
        return false;

    q->events[q->tail] = *evt;

    q->tail = (q->tail + 1) % EVENT_QUEUE_SIZE;
    q->count++;

    return true;
}

bool event_queue_pop(event_queue_t *q, at_event_t *evt)
{
    if (event_queue_is_empty(q))
        return false;

    *evt = q->events[q->head];

    q->head = (q->head + 1) % EVENT_QUEUE_SIZE;
    q->count--;

    return true;
}
