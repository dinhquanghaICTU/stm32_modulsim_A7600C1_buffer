#include "services/sms_recv/sms_recv_service.h"
#include "fsm/sms_recv_fsm.h"
#include <stdbool.h>
#include <stddef.h>

sms_recv_callback_t *sms_recv_cb = NULL;

void sms_recv_service_init(sms_recv_callback_t *cb)
{
    sms_recv_cb = cb;
    sms_recv_fsm_init();
}
