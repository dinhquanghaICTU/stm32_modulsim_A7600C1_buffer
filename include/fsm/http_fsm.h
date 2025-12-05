#ifndef __HTTP_FSM_H__
#define __HTTP_FSM_H__

#include <stdbool.h>
#include <stdint.h>

#include "services/event_queue.h"

typedef enum{
    HTTP_IDLE = 0,
    HTTP_REQ_START,
    HTTP_TERM_FIRST,
    HTTP_INIT,
    HTTP_WAIT_INIT,
    HTTP_SET_UA,
    HTTP_SET_URL,
    HTTP_SET_DATA,
    HTTP_WAIT_DATA,
    HTTP_SEND_DATA,
    HTTP_ACTION,
    HTTP_WAIT_ACTION,
    HTTP_READ,
    HTTP_WAIT_READ,
    HTTP_TERM,
    HTTP_DONE,
    HTTP_ERROR
}http_state_t;

void http_fsm_init(void);
void http_fsm_tick(event_queue_t *q);
void http_set_state(http_state_t st);
http_state_t http_get_state(void);

#endif //__HTTP_FSM_H__

