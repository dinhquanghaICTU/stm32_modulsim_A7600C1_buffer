#include "fsm/sim_fsm.h"
#include "drivers/hardware.h"
#include "drivers/sim_hw.h"
#include "services/uart_channel.h"

static sim_state_t sim_state = SIM_STATE_OFF;
static uint32_t state_timestamp = 0; 

void sim_fsm_init(void)
{
    sim_state = SIM_STATE_OFF;
    state_timestamp = HW_GetTickMs();
}

sim_state_t sim_fsm_get_state(void)
{
    return sim_state;
}

static void sim_set_state(sim_state_t st)
{
    sim_state = st;
    state_timestamp = HW_GetTickMs();

    switch (st)
    {
    case SIM_STATE_WAIT_CPIN:
        uart_channel_send_str(UART_CH_SIM, "AT+CPIN?\r\n");
        break;

    case SIM_STATE_WAIT_CREG:
        uart_channel_send_str(UART_CH_SIM, "AT+CREG?\r\n");
        break;

    default:
        break;
    }
}

void sim_fsm_tick(event_queue_t *q)
{
    at_event_t evt;

    switch (sim_state)
    {
    case SIM_STATE_OFF:
    	onModulSim();
        sim_set_state(SIM_STATE_POWERING_ON);
        break;

    case SIM_STATE_POWERING_ON:
        if (HW_IsTimeout(&state_timestamp, 1500)) {
            sim_set_state(SIM_STATE_WAIT_RDY);
        }
        break;

    case SIM_STATE_WAIT_RDY:


        if (event_queue_pop(q, &evt)) {


            if (evt.type == AT_EVENT_RDY)
                sim_set_state(SIM_STATE_WAIT_CPIN);


            if (evt.type == AT_EVENT_CPIN)
                sim_set_state(SIM_STATE_WAIT_CREG);


            if (evt.type == AT_EVENT_PBREADY)
                sim_set_state(SIM_STATE_WAIT_CPIN);
        }


        if (HW_IsTimeout(&state_timestamp, 3000)) {
            sim_set_state(SIM_STATE_WAIT_CPIN);
        }

        break;

    case SIM_STATE_WAIT_CPIN:
        if (event_queue_pop(q, &evt)) {
            if (evt.type == AT_EVENT_CPIN) {
                sim_set_state(SIM_STATE_WAIT_CREG);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, 2000)) {
            uart_channel_send_str(UART_CH_SIM, "AT+CPIN?\r\n");
        }
        break;

    case SIM_STATE_WAIT_CREG:
        if (event_queue_pop(q, &evt)) {
            if (evt.type == AT_EVENT_CREG && evt.value2 == 1) {
                sim_set_state(SIM_STATE_READY);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, 2000)) {
            uart_channel_send_str(UART_CH_SIM, "AT+CREG?\r\n");
        }
        break;

    case SIM_STATE_READY:

        break;
    }
}
