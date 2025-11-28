#include "fsm/sms_send_fsm.h"
#include "services/sms_send/sms_send_service.h"
#include "services/uart_channel.h"
#include "services/at_parser.h"
#include "drivers/hardware.h"

 sms_send_context_t sms_send_ctx;
 sms_send_callback_t *sms_send_cb;

static sms_send_state_t sms_state = SMS_SEND_IDLE;
static uint32_t state_timestamp = 0;

void sms_send_fsm_init(void)
{
    sms_state = SMS_SEND_IDLE;
    state_timestamp = HW_GetTickMs();
}

sms_send_state_t sms_send_fsm_get_state(void)
{
    return sms_state;
}

void sms_send_set_state(sms_send_state_t st)
{
    sms_state = st;
    state_timestamp = HW_GetTickMs();

    switch(st)
    {
    case SMS_SEND_CMGF:
        uart_channel_send_str(UART_CH_SIM, "AT+CMGF=1\r\n");
        break;

    case SMS_SEND_NUMBER:
        uart_channel_send_format(UART_CH_SIM,"AT+CMGS=\"%s\"\r\n",sms_send_ctx.number);
        break;

    case SMS_SEND_MESSAGE:
        uart_channel_send_str(UART_CH_SIM, sms_send_ctx.message);
        uart_channel_send_byte(UART_CH_SIM, 0x1A);
        break;

    case SMS_SEND_DONE:
        if (sms_send_cb && sms_send_cb->on_sms_sent)
            sms_send_cb->on_sms_sent();
        sms_state = SMS_SEND_IDLE;
        break;

    case SMS_SEND_ERROR:
        if (sms_send_cb && sms_send_cb->on_sms_error)
            sms_send_cb->on_sms_error(-1);
        sms_state = SMS_SEND_IDLE;
        break;

    default:
        break;
    }
}

void sms_send_fsm_tick(event_queue_t *q)
{
    at_event_t evt;

    switch (sms_state)
    {
    case SMS_SEND_REQ:
        sms_send_set_state(SMS_SEND_NUMBER);
        break;

    case SMS_SEND_CMGF:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK)
                sms_send_set_state(SMS_SEND_NUMBER);
            else
                sms_send_set_state(SMS_SEND_ERROR);
        }
        break;

    case SMS_SEND_NUMBER:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_PROMPT)
                sms_send_set_state(SMS_SEND_MESSAGE);
            else if (evt.type == AT_EVENT_ERROR || evt.type == AT_EVENT_CMS_ERROR)
                sms_send_set_state(SMS_SEND_ERROR);
        }
        break;

    case SMS_SEND_MESSAGE:
        sms_send_set_state(SMS_SEND_WAIT_CMGS);
        break;

    case SMS_SEND_WAIT_CMGS:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK)
                sms_send_set_state(SMS_SEND_DONE);

            else if (evt.type == AT_EVENT_CMGS);    

            else if (evt.type == AT_EVENT_ERROR ||
                     evt.type == AT_EVENT_CMS_ERROR)
                sms_send_set_state(SMS_SEND_ERROR);
        }
        break;

    default:
        break;
    }
}

