#include <stdbool.h>
#include <string.h>

#include "drivers/hardware.h"
#include "services/uart_channel.h"
#include "services/at_parser.h"
#include "services/event_queue.h"
#include "services/sms_recv/sms_recv_parser.h"
#include "services/http/http_service.h"
#include "services/http/http_parser.h"
#include "fsm/sim_fsm.h"
#include "fsm/http_fsm.h"
#include "services/ota/ota_service.h"
#include "utils/debug/debug.h"

static void http_on_done(int status, const char *body);
static void http_on_error(int err);
static void ota_on_result(const ota_result_t *result);

int main(void)
{
    
    HW_SystemClockConfig();
    HW_GpioInit();
    HW_DelayInit();
    HW_NvicConfig();

    
    DebugUart_Init();
    SimA7600C1_UartInit();
    uart_channel_init();

    uart_channel_send_str(UART_CH_DEBUG, "\r\n[BOOT]  Test update OTA Starting...\r\n");

    
    event_queue_t evt_q;
    event_queue_init(&evt_q);

    
    sim_fsm_init();

    
    http_callback_t http_cb = {
        .on_http_done = http_on_done,
        .on_http_error = http_on_error,
    };
    http_service_init(&http_cb);

    ota_service_init(ota_on_result);

    uint32_t ota_timestamp = HW_GetTickMs();
    bool ota_requested = false;

    while (1)
    {
        
        process_sim_uart(&evt_q);
        sim_fsm_tick(&evt_q);
        http_fsm_tick(&evt_q);

        
        if (!ota_requested &&sim_fsm_get_state() == SIM_STATE_READY &&HW_IsTimeout(&ota_timestamp, 2000))
        {
            uart_channel_send_str(UART_CH_DEBUG, "[MAIN] SIM READY, starting OTA...\r\n");
            
            if (ota_start_update("https://raw.githubusercontent.com", "/ImBdang/ota/main/main.bin"))
            {
                ota_requested = true;
                uart_channel_send_str(UART_CH_DEBUG, "[MAIN] ota_start_update() OK\r\n");
            }
            else
            {
                uart_channel_send_str(UART_CH_DEBUG, "[MAIN] ota_start_update() FAILED\r\n");
            }
        }

        HW_DelayMs(10);
    }
}

static void http_on_done(int status, const char *body)
{
    uart_channel_send_format(UART_CH_DEBUG,"\r\n[HTTP SUCCESS] status=%d\r\n",status);
}

static void http_on_error(int err)
{
    const char* err_str[] = {
        "NONE",
        "BUSY",
        "TIMEOUT",
        "AT_FAIL",
        "BAD_STATUS"
    };
    uart_channel_send_format(UART_CH_DEBUG,"\r\n[HTTP ERROR] code=%d \r\n",err);
}

static void ota_on_result(const ota_result_t *result)
{
    if (!result)
        return;

    if (result->success)
    {
        uart_channel_send_format(UART_CH_DEBUG,"\r\n[OTA SUCCESS] size=%lu",(unsigned long)result->firmware_size);
    }
    else
    {
        const char* err_str[] = {
            "NONE",
            "BUSY",
            "DOWNLOAD_FAIL",
            "FLASH_WRITE",
            "VERIFY_FAIL",
            "INVALID_SIZE"
        };
        
        uart_channel_send_format(UART_CH_DEBUG,"\r\n[OTA ERROR] code=%d \r\n",result->error);
    }
}
