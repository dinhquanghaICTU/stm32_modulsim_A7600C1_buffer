#include "hardwere.h"

static uint32_t micros = 0;
static uint32_t milis = 0;


//delay
uint32_t Get_SystemClock(void)
{
    return SystemCoreClock;
}

void Delay_Init(void)
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    micros = SystemCoreClock / 1000000;
    milis = SystemCoreClock / 1000;
}

void Delay_ms(uint32_t ms)
{
    uint32_t i;
    for(i = 0; i < ms; i++)
    {
        Delay_us(1000);
    }
}


void delay(uint32_t ms)
{
    Delay_ms(ms);
}

void SystemClock_Config_rieng(void)
{
    RCC_DeInit();


    RCC_HSICmd(ENABLE);

    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_2WS;

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);

    RCC_PLLConfig(RCC_PLLSource_HSI, 16, 336, 4, 7);

    RCC_PLLCmd(ENABLE);

    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while(RCC_GetSYSCLKSource() != 0x08);

    SystemCoreClockUpdate();
}

void Delay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;

    ticks = us * micros;
    told = SysTick->VAL;

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if(tcnt >= ticks)
            {
                break;
            }
        }
    }
}



void onModulSim(void)
{
    GPIO_ResetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin);
    UART_testchuoi(DEBUG, "Modul dang khoi dong\r\n");
    delay(1000);
    GPIO_SetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin);
    UART_testchuoi(DEBUG, "Modul sim dang on\r\n");
    delay(1000);
}

void offModulSim(void)
{
    UART_SendString(DEBUG, "Modul chuan bi tat\r\n");
    GPIO_ResetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin);
    delay(1000);
    GPIO_SetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin);
    UART_SendString(DEBUG, "Modul da tat\r\n");
    delay(1000);
}


void NVIC_config(){
    NVIC_InitTypeDef Nvic_config ;

    Nvic_config.NVIC_IRQChannel = USART1_IRQn;
    Nvic_config.NVIC_IRQChannelPreemptionPriority = 0;
    Nvic_config.NVIC_IRQChannelSubPriority = 0;
    Nvic_config.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&Nvic_config);

    Nvic_config.NVIC_IRQChannel = USART2_IRQn;
    Nvic_config.NVIC_IRQChannelPreemptionPriority = 0;
    Nvic_config.NVIC_IRQChannelSubPriority = 1;
    Nvic_config.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&Nvic_config);
}


void gpio_init (){

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;

    gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //usart os
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //usart modul sim
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    gpio.GPIO_Pin = LED_BLUE_STATEMACHINE_PIN;//led debug
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_8; //nguon
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);
}


//led
void led_debug_on(){
	GPIO_SetBits(GPIOB, LED_BLUE_STATEMACHINE_PIN);
}

void led_debug_off(){
	GPIO_ResetBits(GPIOB, LED_BLUE_STATEMACHINE_PIN);
}

void led_SendUartDone(){
	for (int i =0; i < 3; i++){
		led_debug_on();
		Delay_ms(100);
		led_debug_off();
		Delay_ms(100);
	}
}

//usart_os
void UART2_config_os(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitTypeDef uart_config;
    USART_StructInit(&uart_config);

    uart_config.USART_BaudRate = 115200;
    uart_config.USART_WordLength = USART_WordLength_8b;
    uart_config.USART_StopBits = USART_StopBits_1;
    uart_config.USART_Parity = USART_Parity_No;
    uart_config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart_config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(DEBUG, &uart_config);
    USART_ITConfig(DEBUG, USART_IT_RXNE, ENABLE);
    USART_Cmd(DEBUG, ENABLE);
}

void USART1_Sim_A7600C1_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef uart_config;
    USART_StructInit(&uart_config);

    uart_config.USART_BaudRate = 115200;
    uart_config.USART_WordLength = USART_WordLength_8b;
    uart_config.USART_StopBits = USART_StopBits_1;
    uart_config.USART_Parity = USART_Parity_No;
    uart_config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart_config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &uart_config);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

void UART_testchuoi(USART_TypeDef* USARTx,char *msg)
{

    for(int i = 0; msg[i] != '\0'; i++)
    {
    	USART_SendData(USARTx, msg[i]);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}

void custom_SendByte(USART_TypeDef* USARTx, uint8_t  data){
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(USARTx, data);
}
