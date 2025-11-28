#ifndef __SMS_SEND_FSM_H__
#define __SMS_SEND_FSM_H__

#include "services/event_queue.h"

typedef enum {
    SMS_SEND_IDLE = 0,
    SMS_SEND_REQ,
    SMS_SEND_WAIT_REQ,
    SMS_SEND_CMGF,
    SMS_SEND_NUMBER,
    SMS_SEND_WAIT_PROMPT,
    SMS_SEND_MESSAGE,
    SMS_SEND_WAIT_CMGS,
    SMS_SEND_DONE,
    SMS_SEND_ERROR
} sms_send_state_t;

void sms_send_fsm_init(void);
void sms_send_fsm_tick(event_queue_t *q);
sms_send_state_t sms_send_fsm_get_state(void);
void sms_send_set_state(sms_send_state_t st);

#endif //__SMS_SEND_FSM_H__
