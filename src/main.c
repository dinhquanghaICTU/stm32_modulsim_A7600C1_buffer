#include "hardwere.h"
#include <string.h>


sim_config_t	config_sim;
int main(void)
{
    SystemClock_Config_rieng();
    Delay_Init();  

    gpio_init();
    NVIC_config();
    UART2_config_os();
    USART1_Sim_A7600C1_Config();

    UART_testchuoi(DEBUG, "\r\n=== START ===\r\n");

    sim_state_machine_init();

    while(config_sim.state != SIM_READY && config_sim.state != SIM_ERROR) {
        step_state_machine();
        Delay_ms(100);  
    }
    if(config_sim.state == SIM_READY) {
			UART_testchuoi(DEBUG, "Chuan bi gui SMS...\r\n");
        Delay_ms(2000);

        config_sim.state = SIM_SEND_SMS;

        while(config_sim.state == SIM_SEND_SMS ||
              config_sim.state == SIM_WAIT_SMS_RESPONSE) {
            step_state_machine();
            Delay_ms(100);
        }
    }

    while(1) {
        step_state_machine();
        Delay_ms(100);
    }
}
