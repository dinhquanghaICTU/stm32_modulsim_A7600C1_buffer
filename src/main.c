#include <stdbool.h>

#include "drivers/hardware.h"
#include "services/uart_channel.h"
#include "services/at_parser.h"
#include "services/event_queue.h"
#include "services/http/http_parser.h"
#include "services/http/http_service.h"
#include "fsm/http_fsm.h"
#include "fsm/sim_fsm.h"

#define TEST_HTTP_URL "http://httpbin.org/get"

static void http_done_cb(int status, const char *body)
{
    uart_channel_send_str(UART_CH_DEBUG, "\r\n===== HTTP GET SUCCESS =====\r\n");
    uart_channel_send_format(UART_CH_DEBUG, "HTTP Status: %d\r\n", status);
    uart_channel_send_str(UART_CH_DEBUG, "Response Body:\r\n");
    uart_channel_send_str(UART_CH_DEBUG, body ? body : "<empty>");
    uart_channel_send_str(UART_CH_DEBUG, "\r\n============================\r\n");
}

static void http_error_cb(int err)
{
    uart_channel_send_format(UART_CH_DEBUG, "\r\n[HTTP ERROR] code=%d\r\n", err);
}

static const char* http_state_str(http_state_t state)
{
    switch (state) {
    case HTTP_IDLE:         return "IDLE";
    case HTTP_REQ_START:     return "REQ_START";
    case HTTP_TERM_FIRST:    return "TERM_FIRST";
    case HTTP_INIT:          return "INIT";
    case HTTP_WAIT_INIT:     return "WAIT_INIT";
    case HTTP_SET_CID:      return "SET_CID";
    case HTTP_SET_URL:       return "SET_URL";
    case HTTP_SET_DATA:      return "SET_DATA";
    case HTTP_WAIT_DATA:     return "WAIT_DATA";
    case HTTP_SEND_DATA:    return "SEND_DATA";
    case HTTP_ACTION:        return "ACTION";
    case HTTP_WAIT_ACTION:   return "WAIT_ACTION";
    case HTTP_READ:          return "READ";
    case HTTP_WAIT_READ:     return "WAIT_READ";
    case HTTP_TERM:          return "TERM";
    case HTTP_DONE:          return "DONE";
    case HTTP_ERROR:         return "ERROR";
    default:                 return "UNKNOWN";
    }
}

int main(void)
{
    HW_SystemClockConfig();
    HW_GpioInit();
    HW_DelayInit();
    HW_NvicConfig();

    DebugUart_Init();
    SimA7600C1_UartInit();

    uart_channel_init();

    event_queue_t sim_evt_q;
    event_queue_init(&sim_evt_q);
    event_queue_t http_evt_q;
    event_queue_init(&http_evt_q);

    sim_fsm_init();

    http_callback_t http_cb = {
        .on_http_done = http_done_cb,
        .on_http_error = http_error_cb,
    };
    http_service_init(&http_cb);

    char line[256];
    at_event_t evt;

    uart_channel_send_str(UART_CH_DEBUG, "\r\n=== HTTP GET TEST ===\r\n");
    uart_channel_send_str(UART_CH_DEBUG, "Waiting for SIM ready...\r\n");

    sim_state_t sim_prev = SIM_STATE_OFF;
    http_state_t http_prev = HTTP_IDLE;
    bool http_started = false;
    uint32_t sim_ready_timestamp = 0;

    while (1)
    {
        if (uart_channel_read_line(UART_CH_SIM, line, sizeof(line)))
        {
            uart_channel_send_str(UART_CH_DEBUG, "SIM: ");
            uart_channel_send_str(UART_CH_DEBUG, line);
            uart_channel_send_str(UART_CH_DEBUG, "\r\n");

            if (at_parse_line(line, &evt)) {
                event_queue_push(&sim_evt_q, &evt);
                event_queue_push(&http_evt_q, &evt);
            }

            at_event_t http_evt;
            if (http_parse_line(line, &http_evt)) {
                event_queue_push(&http_evt_q, &http_evt);
            }
        }

        sim_fsm_tick(&sim_evt_q);
        http_fsm_tick(&http_evt_q);

        sim_state_t sim_st = sim_fsm_get_state();
        if (sim_st != sim_prev) {
            sim_prev = sim_st;
            if (sim_st == SIM_STATE_READY) {
                uart_channel_send_str(UART_CH_DEBUG, "\r\n>>> SIM READY!\r\n");
                sim_ready_timestamp = HW_GetTickMs();
            }
        }

        http_state_t http_st = http_get_state();
        if (http_st != http_prev) {
            http_prev = http_st;
            uart_channel_send_format(UART_CH_DEBUG, "[HTTP] State: %s\r\n", http_state_str(http_st));
        }

        // Đợi 2 giây sau khi SIM READY để đảm bảo SIM FSM đã hoàn tất tất cả lệnh
        if (!http_started && sim_st == SIM_STATE_READY && sim_ready_timestamp > 0)
        {
            uint32_t now = HW_GetTickMs();
            if (now - sim_ready_timestamp >= 2000)
            {
                uart_channel_send_format(UART_CH_DEBUG, "\r\n>>> Starting HTTP GET: %s\r\n", TEST_HTTP_URL);
                if (http_get(TEST_HTTP_URL))
                {
                    http_started = true;
                    sim_ready_timestamp = 0; // Reset để không check lại
                }
                else
                {
                    uart_channel_send_str(UART_CH_DEBUG, ">>> HTTP GET failed to start (busy?)\r\n");
                    sim_ready_timestamp = 0; // Reset để không retry
                }
            }
        }

        HW_DelayMs(10);
    }
}
