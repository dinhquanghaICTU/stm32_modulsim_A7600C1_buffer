#include "fsm/http_fsm.h"
#include <string.h>
#include "services/uart_channel.h"
#include "drivers/hardware.h"
#include "services/at_parser.h"
#include "services/http/http_service.h"

extern http_context_t http_ctx;
extern http_callback_t *http_cb;

static http_state_t http_state = HTTP_IDLE;
static uint32_t state_timestamp = 0;

#define HTTP_CMD_TIMEOUT_MS      4000U
#define HTTP_ACTION_TIMEOUT_MS  15000U
#define HTTP_READ_TIMEOUT_MS    15000U

static void http_fail(http_error_t err)
{
    http_ctx.error = err;
    http_set_state(HTTP_ERROR);
}

void http_fsm_init(void)
{
    http_state = HTTP_IDLE;
    state_timestamp = HW_GetTickMs();
}

http_state_t http_get_state(void)
{
    return http_state;
}

void http_set_state(http_state_t st)
{
    http_state = st;
    state_timestamp = HW_GetTickMs();

    switch (st)
    {
    case HTTP_IDLE:
        break;

    case HTTP_REQ_START:
        break;

    case HTTP_TERM_FIRST:
        uart_channel_send_str(UART_CH_SIM, "AT+HTTPTERM\r\n");
        break;

    case HTTP_INIT:
        uart_channel_send_str(UART_CH_SIM, "AT+HTTPINIT\r\n");
        break;

    case HTTP_SET_UA:
        if (http_ctx.user_agent[0] == '\0')
        {
            http_set_state(HTTP_SET_URL);
        }
        else
        {
            uart_channel_send_format(UART_CH_SIM,"AT+HTTPPARA=\"UA\",\"%s\"\r\n",http_ctx.user_agent);
        }
        break;

    case HTTP_SET_URL:
        uart_channel_send_format(UART_CH_SIM,"AT+HTTPPARA=\"URL\",\"%s\"\r\n",http_ctx.url);
        break;

    case HTTP_SET_DATA:
        uart_channel_send_format(UART_CH_SIM,"AT+HTTPDATA=%d,2000\r\n",http_ctx.payload_len);
        break;

    case HTTP_WAIT_DATA:
        break;

    case HTTP_SEND_DATA:
        uart_channel_send_str(UART_CH_SIM, http_ctx.payload);
        break;

    case HTTP_ACTION:
        if (http_ctx.is_post)
            uart_channel_send_str(UART_CH_SIM, "AT+HTTPACTION=1\r\n"); 
        else
            uart_channel_send_str(UART_CH_SIM, "AT+HTTPACTION=0\r\n"); 
        http_state = HTTP_WAIT_ACTION;
        state_timestamp = HW_GetTickMs();
        break;

    case HTTP_WAIT_ACTION:
        break;

    case HTTP_READ:
        if (http_ctx.resp_len > 0)
        {
            uart_channel_send_format(UART_CH_SIM,"AT+HTTPREAD=0,%u\r\n",http_ctx.resp_len);
        }
        else
        {
            uart_channel_send_str(UART_CH_SIM, "AT+HTTPREAD\r\n");
        }
        http_state = HTTP_WAIT_READ;
        state_timestamp = HW_GetTickMs();
        break;

    case HTTP_WAIT_READ:
        break;

    case HTTP_TERM:
        uart_channel_send_str(UART_CH_SIM, "AT+HTTPTERM\r\n");
        break;

    case HTTP_DONE:
        if (http_cb && http_cb->on_http_done)
            http_cb->on_http_done(http_ctx.http_status, http_ctx.response);
        http_state = HTTP_IDLE;
        break;

    case HTTP_ERROR:
        if (http_cb && http_cb->on_http_error)
            http_cb->on_http_error(http_ctx.error);
        http_state = HTTP_IDLE;
        break;
    }
}

void http_fsm_tick(event_queue_t *q)
{
    at_event_t evt;

    switch (http_state)
    {
    case HTTP_IDLE:
        break;

    case HTTP_REQ_START:
        http_set_state(HTTP_TERM_FIRST);
        break;

    case HTTP_TERM_FIRST:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK || evt.type == AT_EVENT_ERROR)
            {
                http_set_state(HTTP_INIT);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_CMD_TIMEOUT_MS))
        {
            http_set_state(HTTP_INIT);
        }
        break;

    case HTTP_INIT:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK)
            {
                http_set_state(HTTP_WAIT_INIT);
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                http_fail(HTTP_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_CMD_TIMEOUT_MS))
        {
            http_fail(HTTP_ERROR_TIMEOUT);
        }
        break;

    case HTTP_WAIT_INIT:
        if (HW_IsTimeout(&state_timestamp, 500))
        {
            http_set_state(HTTP_SET_UA);
        }
        break;

    case HTTP_SET_UA:
        if (http_ctx.user_agent[0] == '\0')
        {
            http_set_state(HTTP_SET_URL);
            break;
        }

        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK || evt.type == AT_EVENT_ERROR)
            {
                http_set_state(HTTP_SET_URL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_CMD_TIMEOUT_MS))
        {
            http_set_state(HTTP_SET_URL);
        }
        break;

    case HTTP_SET_URL:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK)
            {
                if (http_ctx.is_post && http_ctx.payload_len > 0)
                    http_set_state(HTTP_SET_DATA);
                else
                    http_set_state(HTTP_ACTION);
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                http_fail(HTTP_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_CMD_TIMEOUT_MS))
        {
            http_fail(HTTP_ERROR_TIMEOUT);
        }
        break;

    case HTTP_SET_DATA:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_DOWNLOAD)
            {
                http_set_state(HTTP_SEND_DATA);
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                http_fail(HTTP_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_CMD_TIMEOUT_MS))
        {
            http_fail(HTTP_ERROR_TIMEOUT);
        }
        break;

    case HTTP_SEND_DATA:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK)
                http_set_state(HTTP_ACTION);
            else if (evt.type == AT_EVENT_ERROR)
                http_fail(HTTP_ERROR_AT_FAIL);
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_CMD_TIMEOUT_MS))
        {
            http_fail(HTTP_ERROR_TIMEOUT);
        }
        break;

    case HTTP_WAIT_ACTION:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_HTTPACTION)
            {
                http_ctx.http_status = (uint16_t)evt.value1;
                http_ctx.resp_len = (uint16_t)evt.value2;
                http_ctx.resp_received = 0;
                http_ctx.resp_pos = 0;
                http_ctx.response[0] = '\0';

                if (http_ctx.http_status >= 200 && http_ctx.http_status < 400)
                {
                    if (http_ctx.resp_len == 0)
                        http_set_state(HTTP_TERM);
                    else
                        http_set_state(HTTP_READ);
                }
                else
                {
                    http_fail(HTTP_ERROR_BAD_STATUS);
                }
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                http_fail(HTTP_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_ACTION_TIMEOUT_MS))
        {
            http_fail(HTTP_ERROR_TIMEOUT);
        }
        break;

    case HTTP_WAIT_READ:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_HTTPREAD_HEADER)
            {
                http_ctx.resp_len = (uint16_t)evt.value1;
                http_ctx.resp_received = 0;
                http_ctx.resp_pos = 0;
                http_ctx.response[0] = '\0';
            }
            else if (evt.type == AT_EVENT_HTTPREAD_DATA)
            {
                // Dùng length từ parser (value1) thay vì strlen() cho binary data
                size_t chunk_len = (evt.value1 > 0) ? (size_t)evt.value1 : strlen(evt.line);
                if (chunk_len == 0)
                    break;

                http_ctx.resp_received += (uint16_t)chunk_len;

                if (http_ctx.resp_pos < sizeof(http_ctx.response) - 1)
                {
                    size_t remaining = sizeof(http_ctx.response) - 1 - http_ctx.resp_pos;
                    size_t copy_len = chunk_len < remaining ? chunk_len : remaining;

                    if (copy_len > 0)
                    {
                        // Copy binary data, không thêm \n
                        memcpy(&http_ctx.response[http_ctx.resp_pos], evt.line, copy_len);
                        http_ctx.resp_pos += (uint16_t)copy_len;
                    }

                    http_ctx.response[http_ctx.resp_pos] = '\0';
                }
                
                // Debug: log khi nhận data
                uart_channel_send_format(UART_CH_DEBUG, "[HTTP FSM] received %u bytes, total %u/%u, pos=%u\r\n",
                    (unsigned)chunk_len, http_ctx.resp_received, http_ctx.resp_len, http_ctx.resp_pos);
                
                // Nếu đã nhận đủ data, tự động chuyển sang TERM
                if (http_ctx.resp_received >= http_ctx.resp_len && http_ctx.resp_len > 0)
                {
                    uart_channel_send_str(UART_CH_DEBUG, "[HTTP FSM] All data received, moving to TERM\r\n");
                    http_set_state(HTTP_TERM);
                }
            }
            else if (evt.type == AT_EVENT_OK)
            {
                // Chỉ chuyển sang TERM khi đã nhận đủ data
                // (có thể AT_EVENT_OK đến trước khi nhận hết AT_EVENT_HTTPREAD_DATA)
                if (http_ctx.resp_received >= http_ctx.resp_len || http_ctx.resp_len == 0)
                {
                    http_set_state(HTTP_TERM);
                }
                // Nếu chưa đủ, tiếp tục đợi AT_EVENT_HTTPREAD_DATA
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                http_fail(HTTP_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_READ_TIMEOUT_MS))
        {
            http_fail(HTTP_ERROR_TIMEOUT);
        }
        break;

    case HTTP_TERM:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_OK)
                http_set_state(HTTP_DONE);
            else if (evt.type == AT_EVENT_ERROR)
                http_fail(HTTP_ERROR_AT_FAIL);
        }
        else if (HW_IsTimeout(&state_timestamp, HTTP_CMD_TIMEOUT_MS))
        {
            http_fail(HTTP_ERROR_TIMEOUT);
        }
        break;
        
    case HTTP_DONE:
    case HTTP_ERROR:
        break;
    }
}
