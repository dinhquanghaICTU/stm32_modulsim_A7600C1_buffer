#ifndef __SMS_SEND_SERVICE_H__
#define __SMS_SEND_SERVICE_H__

#include <stdbool.h>
#include "services/sms_send/sms_send_callback.h"

typedef struct {
    char number[32];
    char message[161];
} sms_send_context_t;

extern sms_send_context_t sms_send_ctx;
extern sms_send_callback_t *sms_send_cb;

void sms_send_service_init(sms_send_callback_t *cb);

bool sms_send(const char *number, const char *message);

#endif
