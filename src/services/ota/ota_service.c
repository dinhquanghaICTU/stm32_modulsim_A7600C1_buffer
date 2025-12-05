#include "services/ota/ota_service.h"

#include <stdio.h>
#include <string.h>

#include "drivers/flash_wite.h"
#include "services/http/http_service.h"
#include "services/ota/ota_port.h"
#include "services/uart_channel.h"
#include "utils/crc32.h"
#include "stm32f4xx.h"

#ifndef OTA_AUTO_JUMP_AFTER_SUCCESS
#define OTA_AUTO_JUMP_AFTER_SUCCESS    0
#endif

ota_context_t ota_ctx;

static ota_callback_t ota_cb = NULL;
static http_callback_t ota_http_cb;
static http_callback_t *saved_http_cb = NULL;
static bool http_hook_active = false;

static void ota_restore_http_callback(void);
static void ota_handle_http_done(int status, const char *body);
static void ota_handle_http_error(int err);
static void ota_complete(ota_error_t error, bool success);
#if OTA_AUTO_JUMP_AFTER_SUCCESS
static void ota_jump_to_new_firmware(void);
#endif

void ota_service_init(ota_callback_t cb)
{
    memset(&ota_ctx, 0, sizeof(ota_ctx));
    ota_ctx.error = OTA_ERROR_NONE;
    ota_cb = cb;

    ota_http_cb.on_http_done  = ota_handle_http_done;
    ota_http_cb.on_http_error = ota_handle_http_error;
}

bool ota_is_busy(void)
{
    return ota_ctx.in_progress;
}

static bool ota_validate_input(const char *url, const char *path)
{
    if (!url || url[0] == '\0')
        return false;
    if (!path || path[0] == '\0')
        return false;
    if (strlen(url) >= sizeof(ota_ctx.server_url))
        return false;
    if (strlen(path) >= sizeof(ota_ctx.firmware_path))
        return false;
    return true;
}

bool ota_start_update(const char *url, const char *path)
{
    if (ota_ctx.in_progress)
        return false;

    if (!ota_validate_input(url, path))
        return false;

    strncpy(ota_ctx.server_url, url, sizeof(ota_ctx.server_url) - 1);
    ota_ctx.server_url[sizeof(ota_ctx.server_url) - 1] = '\0';

    strncpy(ota_ctx.firmware_path, path, sizeof(ota_ctx.firmware_path) - 1);
    ota_ctx.firmware_path[sizeof(ota_ctx.firmware_path) - 1] = '\0';

    ota_ctx.downloaded_bytes = 0;
    ota_ctx.written_bytes = 0;
    ota_ctx.firmware_size = 0;
    ota_ctx.firmware_crc = 0;
    ota_ctx.error = OTA_ERROR_NONE;

    if (!flash_writer_init())
    {
        ota_complete(OTA_ERROR_FLASH_WRITE, false);
        return false;
    }

    char full_url[sizeof(ota_ctx.server_url) + sizeof(ota_ctx.firmware_path)];
    snprintf(full_url, sizeof(full_url), "%s%s", ota_ctx.server_url, ota_ctx.firmware_path);

    saved_http_cb = http_cb;
    http_cb = &ota_http_cb;
    http_hook_active = true;

    ota_ctx.in_progress = true;

    if (!http_get(full_url))
    {
        ota_restore_http_callback();
        ota_ctx.in_progress = false;
        ota_complete(OTA_ERROR_DOWNLOAD_FAIL, false);
        return false;
    }

    return true;
}

void ota_cancel(void)
{
    if (!ota_ctx.in_progress)
        return;

    ota_restore_http_callback();
    ota_ctx.in_progress = false;
    ota_complete(OTA_ERROR_NONE, false);
}

static void ota_handle_http_done(int status, const char *body)
{
    if (!ota_ctx.in_progress)
        return;

    ota_restore_http_callback();

    // Debug log
    uart_channel_send_format(UART_CH_DEBUG, "[OTA] HTTP done: status=%d, body=%s\r\n", 
                              status, body ? body : "(null)");

    if (status != 200 || !body)
    {
        ota_complete(OTA_ERROR_DOWNLOAD_FAIL, false);
        return;
    }

    const uint8_t *payload = (const uint8_t *)body;
    // Dùng http_ctx.resp_pos thay vì strlen() vì binary data có thể chứa null bytes
    extern http_context_t http_ctx;
    uint32_t payload_len = (uint32_t)http_ctx.resp_pos;

    uart_channel_send_format(UART_CH_DEBUG, "[OTA] payload_len=%lu (from resp_pos)\r\n", (unsigned long)payload_len);

//    if (payload_len == 0U)
//    {
//        ota_complete(OTA_ERROR_INVALID_SIZE, false);
//        return;
//    }

    if (!flash_writer_write(payload, payload_len))
    {
        ota_complete(OTA_ERROR_FLASH_WRITE, false);
        return;
    }

    flash_writer_finish();

    ota_ctx.downloaded_bytes = payload_len;
    ota_ctx.written_bytes = payload_len;
    ota_ctx.firmware_size = payload_len;
    ota_ctx.firmware_crc = crc32_calculate(payload, payload_len);

    ota_complete(OTA_ERROR_NONE, true);

#if OTA_AUTO_JUMP_AFTER_SUCCESS
    ota_jump_to_new_firmware();
#endif
}

static void ota_handle_http_error(int err)
{
    (void)err;

    if (!ota_ctx.in_progress)
        return;

    ota_restore_http_callback();
    ota_complete(OTA_ERROR_DOWNLOAD_FAIL, false);
}

const char *ota_get_status(void)
{
    if (ota_ctx.in_progress)
        return "IN_PROGRESS";

    switch (ota_ctx.error)
    {
    case OTA_ERROR_NONE:          return "IDLE";
    case OTA_ERROR_BUSY:          return "BUSY";
    case OTA_ERROR_DOWNLOAD_FAIL: return "DOWNLOAD_FAIL";
    case OTA_ERROR_FLASH_WRITE:   return "FLASH_WRITE_FAIL";
    case OTA_ERROR_VERIFY_FAIL:   return "VERIFY_FAIL";
    case OTA_ERROR_INVALID_SIZE:  return "INVALID_SIZE";
    default:                      return "UNKNOWN";
    }
}

static void ota_restore_http_callback(void)
{
    if (http_hook_active)
    {
        http_cb = saved_http_cb;
        saved_http_cb = NULL;
        http_hook_active = false;
    }
}

static void ota_complete(ota_error_t error, bool success)
{
    ota_ctx.in_progress = false;
    ota_ctx.error = error;

    if (error != OTA_ERROR_NONE)
    {
        flash_writer_finish();
    }

    if (ota_cb)
    {
        ota_result_t result = {
            .error = error,
            .success = success,
            .firmware_size = ota_ctx.firmware_size,
            .firmware_crc = ota_ctx.firmware_crc,
        };
        ota_cb(&result);
    }
}

#if OTA_AUTO_JUMP_AFTER_SUCCESS
static void ota_jump_to_new_firmware(void)
{
    typedef void (*app_entry_t)(void);
    const uint32_t vector_table = OTA_SLOT_BASE_ADDR;
    const uint32_t sp = *(__IO uint32_t *)vector_table;
    const uint32_t reset = *(__IO uint32_t *)(vector_table + 4U);

    if (sp == 0xFFFFFFFFU || reset == 0xFFFFFFFFU)
        return;

    __disable_irq();
    __set_MSP(sp);
    SCB->VTOR = vector_table;

    app_entry_t entry = (app_entry_t)reset;
    entry();
}
#endif
