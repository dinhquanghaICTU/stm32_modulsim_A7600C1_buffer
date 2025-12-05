#ifndef __OTA_CALLBACK_H__
#define __OTA_CALLBACK_H__

#include "services/ota/ota_service.h"
#include <stdbool.h>

typedef struct
{
    ota_error_t error;
    bool success;
    uint32_t firmware_size;
    uint32_t firmware_crc;
} ota_result_t;

typedef void (*ota_callback_t)(const ota_result_t *result);

#endif //__OTA_CALLBACK_H__