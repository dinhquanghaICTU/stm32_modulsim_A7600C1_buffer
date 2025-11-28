#include "drivers/hardware.h"
#include "services/uart_channel.h"
#include "services/at_parser.h"
#include "services/event_queue.h"
#include "services/sms_recv/sms_recv_service.h"
#include "services/sms_recv/sms_recv_parser.h"
#include "fsm/sim_fsm.h"
#include "fsm/sms_recv_fsm.h"

static void sms_received_cb(const char *phone, const char *msg)
{
    uart_channel_send_str(UART_CH_DEBUG, "\r\n===== SMS RECEIVED =====\r\n");
    uart_channel_send_str(UART_CH_DEBUG, "FROM: ");
    uart_channel_send_str(UART_CH_DEBUG, phone ? phone : "<unknown>");
    uart_channel_send_str(UART_CH_DEBUG, "\r\nTEXT: ");
    uart_channel_send_str(UART_CH_DEBUG, msg ? msg : "<empty>");
    uart_channel_send_str(UART_CH_DEBUG, "\r\n========================\r\n");
}

int main(void)
{
    HW_SystemClockConfig();
    HW_GpioInit();
    HW_DelayInit();
    HW_NvicConfig();

    DebugUart_Init();
    SimA7600C1_UartInit();

    uart_channel_init();

    event_queue_t sim_evt_q;
    event_queue_t sms_recv_q;
    event_queue_init(&sim_evt_q);
    event_queue_init(&sms_recv_q);

    sim_fsm_init();

    sms_recv_callback_t recv_cb = {
        .on_sms_received = sms_received_cb,
    };
    sms_recv_service_init(&recv_cb);

    char line[256];
    at_event_t evt;

    uart_channel_send_str(UART_CH_DEBUG, "=== SIM + SMS RECEIVE TEST ===\r\n");

    sim_state_t sim_prev = SIM_STATE_OFF;

    while (1)
    {
        if (uart_channel_read_line(UART_CH_SIM, line, sizeof(line)))
        {
            uart_channel_send_str(UART_CH_DEBUG, "SIM RAW: ");
            uart_channel_send_str(UART_CH_DEBUG, line);
            uart_channel_send_str(UART_CH_DEBUG, "\r\n");

            if (at_parse_line(line, &evt)) {
                event_queue_push(&sim_evt_q, &evt);
                event_queue_push(&sms_recv_q, &evt);
            }

            at_event_t sms_evt;
            if (sms_recv_parse_line(line, &sms_evt)) {
                event_queue_push(&sms_recv_q, &sms_evt);
            }
        }

        sim_fsm_tick(&sim_evt_q);
        sms_recv_fsm_tick(&sms_recv_q);

        sim_state_t sim_st = sim_fsm_get_state();
        if (sim_st != sim_prev) {
            sim_prev = sim_st;
            uart_channel_send_str(UART_CH_DEBUG, "SIM FSM: ");
            switch (sim_st) {
            case SIM_STATE_OFF:                 uart_channel_send_str(UART_CH_DEBUG, "OFF\r\n"); break;
            case SIM_STATE_POWERING_ON:         uart_channel_send_str(UART_CH_DEBUG, "POWERING_ON\r\n"); break;
            case SIM_STATE_WAIT_RDY:            uart_channel_send_str(UART_CH_DEBUG, "WAIT_RDY\r\n"); break;
            case SIM_STATE_WAIT_CPIN:           uart_channel_send_str(UART_CH_DEBUG, "WAIT_CPIN\r\n"); break;
            case SIM_STATE_WAIT_CREG:           uart_channel_send_str(UART_CH_DEBUG, "WAIT_CREG\r\n"); break;
            case SIM_STATE_CONFIG_SMS_CMGF:     uart_channel_send_str(UART_CH_DEBUG, "CONFIG_CMGF\r\n"); break;
            case SIM_STATE_WAIT_CMGF:           uart_channel_send_str(UART_CH_DEBUG, "WAIT_CMGF\r\n"); break;
            case SIM_STATE_CONFIG_SMS_CSCS:     uart_channel_send_str(UART_CH_DEBUG, "CONFIG_CSCS\r\n"); break;
            case SIM_STATE_WAIT_CSCS:           uart_channel_send_str(UART_CH_DEBUG, "WAIT_CSCS\r\n"); break;
            case SIM_STATE_CONFIG_SMS_CPMS:     uart_channel_send_str(UART_CH_DEBUG, "CONFIG_CPMS\r\n"); break;
            case SIM_STATE_WAIT_CPMS:           uart_channel_send_str(UART_CH_DEBUG, "WAIT_CPMS\r\n"); break;
            case SIM_STATE_CONFIG_SMS_SMEE:     uart_channel_send_str(UART_CH_DEBUG, "CONFIG_CMEE\r\n"); break;
            case SIM_STATE_WAIT_SMEE:           uart_channel_send_str(UART_CH_DEBUG, "WAIT_CMEE\r\n"); break;
            case SIM_STATE_READY:               uart_channel_send_str(UART_CH_DEBUG, "READY!!!\r\n"); break;
            }
        }

        HW_DelayMs(10);
    }
}
