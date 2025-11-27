#ifndef __SIM_FSM_H__
#define __SIM_FSM_H__

#include <stdint.h>
#include <stdbool.h>
#include "services/event_queue.h"

typedef enum {
    SIM_STATE_OFF = 0,
    SIM_STATE_POWERING_ON,
    SIM_STATE_WAIT_RDY,
    SIM_STATE_WAIT_CPIN,
    SIM_STATE_WAIT_CREG,
    SIM_STATE_READY
} sim_state_t;

void sim_fsm_init(void);
void sim_fsm_tick(event_queue_t *q);

sim_state_t sim_fsm_get_state(void);

#endif
