#include "hardwere.h"
#include <stdint.h>
#include <string.h>

static uint32_t micros = 0;
static uint32_t milis = 0;

volatile uint32_t	systick = 0;


sim_config_t	config_sim;

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

    gpio.GPIO_Pin = LED_GREEN_NETWORK_PIN;//led debug
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
	GPIO_SetBits(GPIOB, LED_GREEN_NETWORK_PIN);
}

void led_debug_off(){
	GPIO_ResetBits(GPIOB, LED_GREEN_NETWORK_PIN);
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





uint32_t Gettick(){
	return systick;
}

void sim_state_machine_init(){
	config_sim.max_retry = 3; // thu lai  3 lan
	config_sim.next_State = SIM_IDLE;
	config_sim.retrycount =0;
	config_sim.state= SIM_IDLE ;
	config_sim.timeout = 0;
	config_sim.timestamp = 0;
}

// truyen state and timeout
void set_state (state_machine_t	newState,uint16_t	time_out){
	config_sim.state= newState;
	config_sim.timestamp= Gettick();
	config_sim.timeout= time_out;

	memset((void *) USART1,0, MAX_SIZE);
	uart1_index= 0;
}

// KIEM TRA CHUOI TRA VE CO DUNG KHONG

uint8_t Sim_checkResponse(char * key, uint16_t time_out){
	uint16_t start_Time = Gettick() ;

	while ((Gettick()- start_Time) < time_out){
		if(strstr((char *)USART1, key) != NULL){
				return 1;
			}
		Delay_ms(10);
	}
	return 0;
}

void step_state_machine(){
    static uint8_t check_at_done = 0;
    static uint8_t set_func_done = 0;
    static uint8_t set_charset_done = 0;
    static uint8_t set_SMSMODE_done=0;

    switch(config_sim.state){
        case SIM_IDLE:
            UART_testchuoi(DEBUG, "[IDLE] Bat dau...\r\n");
            led_debug_on();
            check_at_done = 0;
            set_func_done = 0;
            config_sim.retrycount = 0;
            config_sim.state = SIM_POWWER_ON;
            break;

        case SIM_POWWER_ON:
            UART_testchuoi(DEBUG, "[POWER_ON] Bat module...\r\n");
            onModulSim();
            UART_testchuoi(DEBUG, "Doi 15s...\r\n");
            Delay_ms(15000);
            check_at_done = 0;
            config_sim.state = SIM_CHECK_AT;
            break;

        case SIM_CHECK_AT:
            if(check_at_done == 0) {
                UART_testchuoi(DEBUG, "[CHECK_AT] Gui AT...\r\n");
                guiLenhAT("AT");
                check_at_done = 1;
            }

            if(strstr((char*)uart1_dem, "OK") != NULL) {
                UART_testchuoi(DEBUG, "[CHECK_AT] OK!\r\n");
                check_at_done = 0;
                set_func_done = 0;
                config_sim.retrycount = 0;
                config_sim.state = SIM_SET_FULL_FUNC;
            } else {
                config_sim.retrycount++;
                if(config_sim.retrycount >= 3) {
                    UART_testchuoi(DEBUG, "[CHECK_AT] Qua nhieu lan!\r\n");
                    check_at_done = 0;
                    config_sim.state = SIM_ERROR;
                } else {
                    UART_testchuoi(DEBUG, "[CHECK_AT] Thu lai...\r\n");
                    check_at_done = 0;
                    Delay_ms(2000);
                }
            }
            break;

        case SIM_SET_FULL_FUNC:
            if(set_func_done == 0) {
                UART_testchuoi(DEBUG, "[SET_FUNC] AT+CFUN=1...\r\n");
                guiLenhAT("AT+CFUN=1");
                set_func_done = 1;
            }

            if(strstr((char*)uart1_dem, "OK") != NULL) {
                UART_testchuoi(DEBUG, "[SET_FUNC] OK!\r\n");
                check_at_done = 0;
                set_func_done = 0;
                config_sim.retrycount = 0;

                config_sim.state = SIM_SET_CHARSET;
            } else {
                UART_testchuoi(DEBUG, "[SET_FUNC] FAILED!\r\n");
                set_func_done = 0;
                config_sim.state = SIM_ERROR;
            }
            break;


        case SIM_SET_CHARSET:
        	if(set_charset_done == 0){
        		 UART_testchuoi(DEBUG, "[CHARSET] AT+CSCS...\r\n");
        		 guiLenhAT("AT+CSCS=\"IRA\"");
        		 set_charset_done=1;
        	}
        	if(Sim_checkResponse("OK", 2000)) {
        	     UART_testchuoi(DEBUG, "[CHARSET] OK\r\n");
        	     check_at_done = 0;
        	     set_func_done = 0;
                 config_sim.retrycount = 0;
        	     set_charset_done = 0;
        	     config_sim.state= SIM_SET_SMS_MODE;


       	    } else {
       	    	set_charset_done = 0;
       	    	UART_testchuoi(DEBUG, "[CHARSET] FAILED\r\n");
       	    	config_sim.state = SIM_ERROR;
            }
            break;


        case SIM_SET_SMS_MODE:
        	if(set_SMSMODE_done == 0){
        		UART_testchuoi(DEBUG, "[SMS_MODE] AT+CMGF=1...\r\n");
        		guiLenhAT("AT+CMGF=1");
        		set_SMSMODE_done=1;
        	}
        	if(Sim_checkResponse("OK", 2000)) {
        		check_at_done = 0;
        		set_func_done = 0;
                config_sim.retrycount = 0;
        		set_SMSMODE_done=0;
        	    UART_testchuoi(DEBUG, "[SMS_MODE] OK\r\n");
                config_sim.state=SIM_CHECK_NETWORK;
        	} else {
                UART_testchuoi(DEBUG, "[SMS_MODE] FAILED\r\n");
                config_sim.state=SIM_ERROR;
       	     }
        	            break;





        case SIM_READY:
            UART_testchuoi(DEBUG, "\r\n=== MODULE SAN SANG ===\r\n");
            led_debug_off();
            // Dừng lại
            break;

        case SIM_ERROR:
            UART_testchuoi(DEBUG, "[ERROR] Loi!\r\n");
            for(int i = 0; i < 5; i++){
                led_debug_on();
                Delay_ms(100);
                led_debug_off();
                Delay_ms(100);
            }
            check_at_done = 0;
            set_func_done = 0;
            Delay_ms(5000);
            config_sim.state = SIM_IDLE;
            break;

        default:
            config_sim.state = SIM_IDLE;
            break;
    }
}





