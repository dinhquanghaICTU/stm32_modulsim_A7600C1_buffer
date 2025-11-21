#include "hardwere.h"
#include <string.h>
#include "ringbuffer.h"


sim_config_t	config_sim;

int main(void)
{
    SystemClock_Config_rieng();
    Delay_Init();
    ringBuff_init(&BUFFER_DEBUG, DEBUG_Data, 256, 1);
    ringBuff_init(&BUFFER_AT, AT_Data, 256, 1);
    gpio_init();
    NVIC_config();
    UART2_config_os();
    USART1_Sim_A7600C1_Config();
    
    onModulSim();
    Delay_ms(15000);

    guiLenhAT("AT"); 
    guiLenhAT("AT+CPIN?");
    guiLenhAT("AT+CSQ");
    guiLenhAT("AT+CREG?");
    
    guiLenhAT("AT+CPMS=\"SM\",\"SM\",\"SM\"");   

    guiLenhAT("AT+CMGF=1");                    
    guiLenhAT("AT+CSCS=\"GSM\"");            
    
    gui_tinnhan("0386126985","alo alo alo");
    while(1) {
        
    }
}
