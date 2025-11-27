#include "drivers/hardware.h"
#include "services/uart_channel.h"
#include "services/at_parser.h"
#include "services/event_queue.h"
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

    event_queue_t evtQ;
    event_queue_init(&evtQ);

    sim_fsm_init();

    char line[256];
    at_event_t evt;

    uart_channel_send_str(UART_CH_DEBUG, "=== SIM TEST START ===\r\n");

    sim_state_t prev = SIM_STATE_OFF;

    while (1)
    {
        // 1) đọc line từ SIM
        if (uart_channel_read_line(UART_CH_SIM, line, sizeof(line)))
        {
            uart_channel_send_str(UART_CH_DEBUG, "SIM RAW: ");
            uart_channel_send_str(UART_CH_DEBUG, line);
            uart_channel_send_str(UART_CH_DEBUG, "\r\n");

            // 2) parse thành event (sau này mình viết at_parser)
            if (at_parse_line(line, &evt)) {
                event_queue_push(&evtQ, &evt);
            }
        }

        // 3) chạy FSM
        sim_fsm_tick(&evtQ);

        // 4) log state khi đổi
        sim_state_t st = sim_fsm_get_state();
        if (st != prev) {
            prev = st;
            uart_channel_send_str(UART_CH_DEBUG, "FSM STATE: ");
            switch (st) {
            case SIM_STATE_OFF:          uart_channel_send_str(UART_CH_DEBUG, "OFF\r\n"); break;
            case SIM_STATE_POWERING_ON:  uart_channel_send_str(UART_CH_DEBUG, "POWERING_ON\r\n"); break;
            case SIM_STATE_WAIT_RDY:     uart_channel_send_str(UART_CH_DEBUG, "WAIT_RDY\r\n"); break;
            case SIM_STATE_WAIT_CPIN:    uart_channel_send_str(UART_CH_DEBUG, "WAIT_CPIN\r\n"); break;
            case SIM_STATE_WAIT_CREG:    uart_channel_send_str(UART_CH_DEBUG, "WAIT_CREG\r\n"); break;
            case SIM_STATE_READY:        uart_channel_send_str(UART_CH_DEBUG, "READY!!!\r\n"); break;
            }
        }
    }
}
