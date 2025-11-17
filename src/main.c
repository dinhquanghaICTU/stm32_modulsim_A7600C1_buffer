#include "hardwere.h"
#include <string.h>

volatile uint8_t uart1_dem[MAX_SIZE];
volatile uint16_t uart1_index = 0;
volatile uint8_t uart2_dem[MAX_SIZE];
volatile uint16_t uart2_index = 0;

int main(void)
{
    SystemClock_Config_rieng();
    Delay_Init();
    gpio_init();
    NVIC_config();
    UART2_config_os();
    USART1_Sim_A7600C1_Config();

    UART_testchuoi(DEBUG, "\r\n A7600C1 SMS \r\n");

    onModulSim();
    Delay_ms(15000);

    guiLenhAT("AT");
    Delay_ms(1000);

    guiLenhAT("AT+CFUN=1");
    Delay_ms(5000);

    guiLenhAT("AT+CSCS=\"IRA\"");
    Delay_ms(1000);

    guiLenhAT("AT+CMGF=1");
    Delay_ms(1000);

    guiLenhAT("AT+COPS=0");
    Delay_ms(8000);

    UART_testchuoi(DEBUG, "GUI SMS ");
    test_tinnhan("0355549165", "DINH QUANG HA DANG TEST MODUL SIM A7600C1");

    UART_testchuoi(DEBUG, "XONG \r\n");

    for(int i = 0; i < 5; i++) {
        led_debug_on();
        Delay_ms(100);
        led_debug_off();
        Delay_ms(100);
    }

    while(1) {
        Delay_ms(1000);
    }
}
