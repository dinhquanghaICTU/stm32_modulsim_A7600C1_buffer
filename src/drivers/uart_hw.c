#include "drivers/uart_hw.h"
#include "services/uart_channel.h"
#include "stm32f4xx.h"

void uart_hw_write(USART_TypeDef *uart, const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        while (!(uart->SR & USART_SR_TXE)); 
        uart->DR = data[i];
    }

    while (!(uart->SR & USART_SR_TC));
}

void uart_hw_irq_rx(USART_TypeDef *uart, uart_channel_t ch)
{
    if (USART_GetITStatus(uart, USART_IT_RXNE) != RESET) {

        uint8_t b = USART_ReceiveData(uart);

        uart_channel_isr_rx(ch, b);
    }
}
