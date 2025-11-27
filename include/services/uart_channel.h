#ifndef __UART_CHANNEL_H__
#define __UART_CHANNEL_H__

#include "utils/ringbuff.h"


typedef enum {
    UART_CH_DEBUG =0,
    UART_CH_SIM,
}uart_channel_t ;


void uart_channel_init(void);

#endif //__UART_CHANNEL_H__