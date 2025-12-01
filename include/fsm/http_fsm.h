#ifndef __HTTP_FSM_H__
#define __HTTP_FSM_H__

#include <stdbool.h>
#include <stdint.h>

#include "services/event_queue.h"

typedef enum{
    HTTP_IDLE = 0,
    HTTP_REQ_START,
    HTTP_TERM_FIRST,    // AT+HTTPTERM (để đảm bảo clean state trước khi init)

    HTTP_INIT,          // AT+HTTPINIT
    HTTP_WAIT_INIT,     // Đợi sau HTTPINIT để module sẵn sàng
    HTTP_SET_CID,       // AT+HTTPPARA="CID",1
    HTTP_SET_URL,       // AT+HTTPPARA="URL","..."

    HTTP_SET_DATA,      // AT+HTTPDATA=...
    HTTP_WAIT_DATA,     // chờ DOWNLOAD / prompt
    HTTP_SEND_DATA,     // gửi payload POST

    HTTP_ACTION,        // AT+HTTPACTION=0/1
    HTTP_WAIT_ACTION,   // đợi +HTTPACTION

    HTTP_READ,          // AT+HTTPREAD
    HTTP_WAIT_READ,     // đợi +HTTPREAD và body

    HTTP_TERM,          // AT+HTTPTERM
    HTTP_DONE,
    HTTP_ERROR
}http_state_t;

void http_fsm_init(void);
void http_fsm_tick(event_queue_t *q);
void http_set_state(http_state_t st);
http_state_t http_get_state(void);



#endif //__HTTP_FSM_H__