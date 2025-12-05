#ifndef __UART_CHANNEL_H__
#define __UART_CHANNEL_H__

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    UART_CH_DEBUG = 0,
    UART_CH_SIM,
} uart_channel_t;

void uart_channel_init(void);

void uart_channel_isr_rx(uart_channel_t ch, uint8_t byte);

void uart_channel_send(uart_channel_t ch, const uint8_t *data, uint16_t len);
void uart_channel_send_str(uart_channel_t ch, const char *str);
void uart_channel_send_format(uart_channel_t ch, const char *fmt, ...);
void uart_channel_send_byte(uart_channel_t ch, uint8_t b);

bool uart_channel_read_byte(uart_channel_t ch, uint8_t *out);
bool uart_channel_read_line(uart_channel_t ch, char *out, uint16_t max_len);
uint16_t uart_channel_read_bytes(uart_channel_t ch, uint8_t *out, uint16_t max_len);

#endif //__UART_CHANNEL_H__