#include "services/uart_channel.h"
#include "drivers/uart_hw.h"
#include "drivers/hardware.h"
#include "utils/ringbuff.h"
#include <string.h>

static ringbuff_t rb_debug;
static ringbuff_t rb_sim;

static uint8_t rb_debug_buf[256];
static uint8_t rb_sim_buf[512];

void uart_channel_init(void)
{
    ringbuff_init(&rb_debug, rb_debug_buf, sizeof(rb_debug_buf));
    ringbuff_init(&rb_sim,   rb_sim_buf,   sizeof(rb_sim_buf));
}

// ISR gọi hàm này để nhét byte vào buffer
void uart_channel_isr_rx(uart_channel_t ch, uint8_t byte)
{
    if (ch == UART_CH_SIM)
        ringbuff_write(&rb_sim, &byte, 1);
    else
        ringbuff_write(&rb_debug, &byte, 1);
}

// Gửi raw data
void uart_channel_send(uart_channel_t ch, const uint8_t *data, uint16_t len)
{
    if (ch == UART_CH_SIM)
        uart_hw_write(UART_SIM, data, len);
    else
        uart_hw_write(UART_DEBUG, data, len);
}

// Gửi string (chính là hàm mà main đang gọi)
void uart_channel_send_str(uart_channel_t ch, const char *str)
{
    uart_channel_send(ch, (const uint8_t*)str, (uint16_t)strlen(str));
}

// Đọc 1 byte từ ringbuffer (non-blocking)
bool uart_channel_read_byte(uart_channel_t ch, uint8_t *out)
{
    if (ch == UART_CH_SIM)
        return ringbuff_read(&rb_sim, out, 1) == 1;
    else
        return ringbuff_read(&rb_debug, out, 1) == 1;
}

// Đọc 1 dòng kết thúc bằng '\n'
bool uart_channel_read_line(uart_channel_t ch, char *out, uint16_t max_len)
{
    static char line_buf[256];
    static uint16_t line_len = 0;

    uint8_t c;

    while (uart_channel_read_byte(ch, &c))
    {
        if (c == '\n') {

            line_buf[line_len] = 0;

            if (line_len > 0) {
                strncpy(out, line_buf, max_len);
                line_len = 0;
                return true;
            }

            line_len = 0;
        }
        else if (c != '\r') {

            if (line_len < sizeof(line_buf) - 1)
                line_buf[line_len++] = c;
            else
                line_len = 0;   // overflow thì reset
        }
    }

    return false;
}
