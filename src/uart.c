// ========== interrupt.c ==========
#include "hardwere.h"

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);

        // Forward tới DEBUG UART
        custom_SendByte(DEBUG, data);

        // Lưu vào buffer
        uart1_dem[uart1_index++] = data;
        if(uart1_index >= MAX_SIZE)
            uart1_index = 0;

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

        uart2_dem[uart2_index++] = data;
        if(uart2_index >= MAX_SIZE)
            uart2_index = 0;

        USART_ClearITPendingBit(DEBUG, USART_IT_RXNE);
    }
}
