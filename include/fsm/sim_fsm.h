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
    
    // Network setup (PDP context)
    SIM_STATE_CONFIG_NETWORK_ATTACH,    // AT+CGATT=1
    SIM_STATE_WAIT_NETWORK_ATTACH,
    SIM_STATE_CONFIG_PDP_CONTEXT,       // AT+CGDCONT=1,"IP","internet"
    SIM_STATE_WAIT_PDP_CONTEXT,
    SIM_STATE_ACTIVATE_PDP,              // AT+CGACT=1,1
    SIM_STATE_WAIT_ACTIVATE_PDP,
    
    // SMS config
    SIM_STATE_CONFIG_SMS_CMGF,
    SIM_STATE_WAIT_CMGF,
    SIM_STATE_CONFIG_SMS_CSCS,
    SIM_STATE_WAIT_CSCS,
    SIM_STATE_CONFIG_SMS_CPMS,
    SIM_STATE_WAIT_CPMS,
    SIM_STATE_CONFIG_SMS_SMEE,
    SIM_STATE_WAIT_SMEE,
    SIM_STATE_READY
} sim_state_t;

void sim_fsm_init(void);
void sim_fsm_tick(event_queue_t *q);
static void sim_set_state(sim_state_t st);
sim_state_t sim_fsm_get_state(void);

#endif
