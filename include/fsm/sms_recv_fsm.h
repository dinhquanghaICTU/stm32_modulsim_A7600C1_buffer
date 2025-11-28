#ifndef __SMS_RECV_FSM_H__
#define __SMS_RECV_FSM_H__

#include <stdint.h>
#include "services/event_queue.h"

// Trạng thái FSM nhận SMS
typedef enum {
    SMS_RECV_IDLE = 0,
    SMS_RECV_WAIT_HEADER,
    SMS_RECV_WAIT_TEXT,
    SMS_RECV_DONE
} sms_recv_state_t;

void sms_recv_fsm_init(void);
void sms_recv_fsm_tick(event_queue_t *q);
void sms_recv_set_state(sms_recv_state_t st);

#endif /* __SMS_RECV_FSM_H__ */
