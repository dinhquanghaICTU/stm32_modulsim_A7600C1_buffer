#ifndef __SMS_RECV_SERVICE_H__
#define __SMS_RECV_SERVICE_H__

#include "services/sms_recv/sms_recv_callback.h"

extern sms_recv_callback_t *sms_recv_cb;

void sms_recv_service_init(sms_recv_callback_t *cb);

#endif /* __SMS_RECV_SERVICE_H__ */
