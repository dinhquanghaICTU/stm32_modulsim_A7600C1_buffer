#ifndef __UART_HW_H__
#define __UART_HW_H__

#include <stdint.h>
#include "stm32f4xx.h"

void uart_hw_write(USART_TypeDef *uart, const uint8_t *data, uint16_t len);

#endif
