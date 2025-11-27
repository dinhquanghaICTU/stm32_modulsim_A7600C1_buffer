#include "drivers/hardware.h"

volatile uint32_t system_tick_ms = 0;

/*===========================================================
 *  RETURN SYSTEM CLOCK
 *===========================================================*/
uint32_t HW_GetSystemClock(void)
{
    return SystemCoreClock;
}

/*===========================================================
 *  SYSTICK INIT (1ms)
 *===========================================================*/
void HW_DelayInit(void)
{
    // SysTick chạy mỗi 1ms
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        while (1); // lỗi thì đứng luôn
    }
}

/*===========================================================
 *  SYSTICK INTERRUPT
 *===========================================================*/


/*===========================================================
 *  GET CURRENT TIME (ms)
 *===========================================================*/
uint32_t HW_GetTickMs(void)
{
    return system_tick_ms;
}

/*===========================================================
 *  BLOCK DELAY (ms)
 *===========================================================*/
void HW_DelayMs(uint32_t ms)
{
    uint32_t start = HW_GetTickMs();
    while (HW_GetTickMs() - start < ms);
}

void HW_Delay(uint32_t ms)
{
    HW_DelayMs(ms);
}



bool HW_IsTimeout(uint32_t *timestamp, uint32_t period_ms)
{
    uint32_t now = HW_GetTickMs();
    if (now - *timestamp >= period_ms)
    {
        *timestamp = now;  // cập nhật mốc mới
        return true;
    }
    return false;
}

/*===========================================================
 *  SYSTEM CLOCK CONFIG
 *===========================================================*/
void HW_SystemClockConfig(void)
{
    RCC_DeInit();

    RCC_HSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_2WS;

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);

    RCC_PLLConfig(RCC_PLLSource_HSI, 16, 336, 4, 7);
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);

    SystemCoreClockUpdate();
}

/*===========================================================
 * NVIC CONFIG FOR USART
 *===========================================================*/
void HW_NvicConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // USART1 - SIM
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // USART2 - DEBUG
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

}

/*===========================================================
 * GPIO INIT
 *===========================================================*/
void HW_GpioInit(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;

    // USART2 (PA2-TX, PA3-RX)
    gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    // USART1 (PA9-TX, PA10-RX)
    gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    // LED NETWORK (PB9)
    gpio.GPIO_Pin = LED_GREEN_NETWORK_PIN;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);

    // SIM PWRKEY
    gpio.GPIO_Pin = GSM_PWKEY_Pin ;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GSM_PWKEY_GPIO, &gpio);
}

/*===========================================================
 *  DEBUG UART INIT (USART2)
 *===========================================================*/
void DebugUart_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitTypeDef uart;

    uart.USART_BaudRate = 115200;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(UART_DEBUG, &uart);
    USART_Cmd(UART_DEBUG, ENABLE);

    USART_ITConfig(UART_DEBUG, USART_IT_RXNE, ENABLE);
}

/*===========================================================
 *  SIM UART INIT (USART1)
 *===========================================================*/
void SimA7600C1_UartInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef uart;

    uart.USART_BaudRate = 115200;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(UART_SIM, &uart);
    USART_Cmd(UART_SIM, ENABLE);

    USART_ITConfig(UART_SIM, USART_IT_RXNE, ENABLE);
}
