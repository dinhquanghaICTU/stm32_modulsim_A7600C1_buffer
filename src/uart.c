
#include "hardwere.h"

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);

        // Gửi echo sang UART debug
        custom_SendByte(DEBUG, data);

        // Ghi vào ring buffer AT
        ringbuff_write(&BUFFER_AT, 1, &data);

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}


void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(DEBUG, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(DEBUG);

        // Forward tới SIM module
        custom_SendByte(USART1, data);


        ringbuff_write(&BUFFER_AT, 1, &data);

        uart2_dem[uart2_index++] = data;
        if(uart2_index >= MAX_SIZE)
            uart2_index = 0;

        USART_ClearITPendingBit(DEBUG, USART_IT_RXNE);
    }
}



void handler_TX(USART_TypeDef* USARTx, RINGBUFFER_t *buffer)
{
    uint8_t data;
    while(ringbuff_pop(buffer, &data))
    {
        custom_SendByte(USARTx, data);
    }
}