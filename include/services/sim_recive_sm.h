#ifndef __SMS_RECIVE_SM_H
#define __SMS_RECIVE_SM_H

#include "drivers/hardwere.h"
#include "services/uart_channel.h"

typedef enum{
    SMS_RECEIVE_IDLE = 0,
    SMS_RECEIVE_SET_CMGF,
    SMS_RECEIVE_WAIT_CMGF,
    SMS_RECEIVE_SET_CNMI,
    SMS_RECEIVE_WAIT_CNMI,
    SMS_RECEIVE_SHOW_ALL,
    SMS_RECEIVE_WAIT_SHOW_ALL,
    SMS_RECEIVE_REMOVE,
    SMS_RECEIVE_WAIT_REMOVE,
    SMS_RECEIVE_DONE,
    SMS_RECEIVE_ERROR
} sms_recive_sm_t;

typedef void (*sms_receive_callback_t)(void);

void sms_recive_init(void);

void sms_recive_process(void);

void sms_receive_sim_rx_callback(uart_channel_t channel);

sms_recive_sm_t sms_recive_get_state(void);

void sms_receive_set_callback(sms_receive_callback_t callback);

void sms_receive_request_start(void);

#endif //__SIM_RECIVE_SM_H