#ifndef __SMS_RECV_CALLBACK_H__
#define __SMS_RECV_CALLBACK_H__

typedef struct
{
    void (*on_sms_received)(const char *phone, const char *msg);
} sms_recv_callback_t;

#endif /* __SMS_RECV_CALLBACK_H__ */
