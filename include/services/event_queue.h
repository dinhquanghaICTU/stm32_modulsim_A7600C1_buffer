#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "at_parser.h"

#define EVENT_QUEUE_SIZE 16

typedef struct {
    at_event_t events[EVENT_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} event_queue_t;

void event_queue_init(event_queue_t *q);

bool event_queue_push(event_queue_t *q, const at_event_t *evt);
bool event_queue_pop(event_queue_t *q, at_event_t *evt);

bool event_queue_is_empty(event_queue_t *q);
bool event_queue_is_full(event_queue_t *q);

#endif
