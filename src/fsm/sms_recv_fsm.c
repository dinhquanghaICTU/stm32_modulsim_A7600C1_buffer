#include "fsm/sms_recv_fsm.h"
#include "services/sms_recv/sms_recv_service.h"
#include "services/uart_channel.h"
#include "services/sms_recv/sms_recv_parser.h"
#include "drivers/hardware.h"
#include <string.h>

extern sms_recv_callback_t *sms_recv_cb;

// ======================================================================
// INTERNAL STATE
// ======================================================================
static sms_recv_state_t recv_state = SMS_RECV_IDLE;
static uint32_t state_timestamp = 0;

static int sms_index = -1;
static char sms_phone[32];
static char sms_text[256];

// ======================================================================
// INIT
// ======================================================================
void sms_recv_fsm_init(void)
{
    recv_state = SMS_RECV_IDLE;
    state_timestamp = HW_GetTickMs();
}

// ======================================================================
// SET STATE
// ======================================================================
void sms_recv_set_state(sms_recv_state_t st)
{
    recv_state = st;
    state_timestamp = HW_GetTickMs();
}

// ======================================================================
// FSM TICK
// ======================================================================
void sms_recv_fsm_tick(event_queue_t *q)
{
    at_event_t evt;

    switch (recv_state)
    {
    // ==================================================================
    // 1) CHỜ URC BÁO CÓ TIN NHẮN MỚI
    // ==================================================================
    case SMS_RECV_IDLE:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_CMTI)
            {
                sms_index = evt.value1;   // lưu index SMS

                // Gửi lệnh đọc SMS
                uart_channel_send_format(UART_CH_SIM,
                                         "AT+CMGR=%d\r\n",
                                         sms_index);

                sms_recv_set_state(SMS_RECV_WAIT_HEADER);
            }
        }
        break;

    // ==================================================================
    // 2) CHỜ HEADER CMGR (CHỨA SỐ ĐIỆN THOẠI)
    // ==================================================================
    case SMS_RECV_WAIT_HEADER:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_CMGR_HEADER)
            {
                // Example:
                // +CMGR: "REC READ","+8498xxxxxx",,"23/01/12,12:30:00+28"
                const char *p = strchr(evt.line, '"');
                if (p)
                {
                    p++;  // bỏ dấu "
                    const char *end = strchr(p, '"');
                    if (end)
                    {
                        uint16_t len = end - p;
                        if (len < sizeof(sms_phone))
                        {
                            memcpy(sms_phone, p, len);
                            sms_phone[len] = 0;
                        }
                    }
                }

                sms_recv_set_state(SMS_RECV_WAIT_TEXT);
            }
        }
        break;

    // ==================================================================
    // 3) CHỜ TEXT NỘI DUNG TIN NHẮN
    // ==================================================================
    case SMS_RECV_WAIT_TEXT:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_CMGR_TEXT)
            {
                strncpy(sms_text, evt.line, sizeof(sms_text));
            }
            else if (evt.type == AT_EVENT_OK)
            {
                sms_recv_set_state(SMS_RECV_DONE);
            }
        }
        break;

    // ==================================================================
    // 4) HOÀN THÀNH → GỌI CALLBACK
    // ==================================================================
    case SMS_RECV_DONE:
        if (sms_recv_cb && sms_recv_cb->on_sms_received)
            sms_recv_cb->on_sms_received(sms_phone, sms_text);

        // reset state
        recv_state = SMS_RECV_IDLE;
        break;
    }
}
