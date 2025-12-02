#include <stdbool.h>

#include "drivers/hardware.h"
#include "services/uart_channel.h"
#include "services/at_parser.h"
#include "services/event_queue.h"
#include "services/mqtt/mqtt_parser.h"
#include "services/mqtt/mqtt_service.h"
#include "fsm/mqtt_fsm.h"
#include "fsm/sim_fsm.h"



int main(void)
{
    HW_SystemClockConfig();
    HW_GpioInit();
    HW_DelayInit();
    HW_NvicConfig();

    DebugUart_Init();
    SimA7600C1_UartInit();
    uart_channel_init();

    

    while (1)
    {
        HW_DelayMs(10);
    }
}
