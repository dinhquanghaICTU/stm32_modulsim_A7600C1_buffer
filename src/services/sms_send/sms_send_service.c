#include "services/sms_send/sms_send_service.h"
#include "fsm/sms_send_fsm.h"
#include <string.h>


sms_send_context_t sms_send_ctx;
sms_send_callback_t *sms_send_cb = NULL;

void sms_send_service_init(sms_send_callback_t *cb)
{
    sms_send_cb = cb;
    sms_send_fsm_init();
}

bool sms_send(const char *number, const char *message)
{
    if (sms_send_fsm_get_state() != SMS_SEND_IDLE)
        return false;   

    strncpy(sms_send_ctx.number, number, sizeof(sms_send_ctx.number) - 1);
    sms_send_ctx.number[sizeof(sms_send_ctx.number) - 1] = '\0';

    strncpy(sms_send_ctx.message, message, sizeof(sms_send_ctx.message) - 1);
    sms_send_ctx.message[sizeof(sms_send_ctx.message) - 1] = '\0';

    sms_send_set_state(SMS_SEND_REQ);
    return true;
}
