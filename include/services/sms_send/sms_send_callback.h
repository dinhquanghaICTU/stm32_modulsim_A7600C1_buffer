#ifndef __SMS_SEND_CALLBACK_H__
#define __SMS_SEND_CALLBACK_H__

typedef struct
{
    void (*on_sms_sent)(void);        // SMS gửi thành công
    void (*on_sms_error)(int err);    // SMS gửi thất bại
} sms_send_callback_t;

#endif
