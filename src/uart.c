
#include "hardwere.h"

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        
        uart1_dem[uart1_index++] = data;
        if(uart1_index >= MAX_SIZE)
            uart1_index = 0;

        USART_ClearITPendingBit(AT, USART_IT_RXNE);
    }
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(DEBUG, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(DEBUG);

        
        custom_SendByte(AT, data);

        
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

void UART_testchuoi(RINGBUFFER_t *buffer , char *msg)
{

    for(int i = 0; msg[i] != '\0'; i++)
    {
        ringbuff_write(buffer, 1, (uint8_t *)&msg[i]);
    }
}

void custom_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, data);;
}



