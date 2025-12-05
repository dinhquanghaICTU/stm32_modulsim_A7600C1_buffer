#ifndef __OTA_SERVICE_H__
#define __OTA_SERVICE_H__

#include <stdbool.h>
#include <stdint.h>

#include "services/ota/ota_type.h"
#include "services/ota/ota_callback.h"

typedef struct
{
    char server_url[256];
    char firmware_path[128];
    uint32_t firmware_size;
    uint32_t firmware_crc;
    uint32_t downloaded_bytes;
    uint32_t written_bytes;
    ota_error_t error;
    bool in_progress;
} ota_context_t;

void ota_service_init(ota_callback_t cb);
bool ota_is_busy(void);
bool ota_start_update(const char *url, const char *path);
void ota_cancel(void);
const char *ota_get_status(void);

#endif