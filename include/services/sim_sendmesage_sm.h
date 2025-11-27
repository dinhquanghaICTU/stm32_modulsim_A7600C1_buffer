#ifndef __SIM_SEND_MESSAGE_H__
#define __SIM_SEND_MESSAGE_H__
#include "stdbool.h"
#include "stdint.h"
#include "drivers/hardwere.h"
#include "uart_channel.h"


typedef enum{
    SIM_SEND_SMS_IDLE,
    SIM_SEND_SMS_CMGF,
    SIM_SEND_SMS_WAIT_CMGF,
    SIM_SEND_SMS_CMGS,
    SIM_SEND_SMS_WAIT_PROMPT,
    SIM_SEND_SMS_DATA,
    SIM_SEND_SMS_WAIT_RESULT,
    SIM_SEND_SMS_DONE,
    SIM_SEND_SMS_ERROR
}sim_send_message_t;

void sim_send_sms_init(void);
void sim_send_sms_process(void);
sim_send_message_t sim_send_sms_get_state(void);
void sim_send_sms_request(const char* phone_number, const char* message);




#endif //__SIM_SEND_MESSAGE_H__