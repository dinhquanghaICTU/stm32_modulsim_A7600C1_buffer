#ifndef __HTTP_SERVICE_H__
#define __HTTP_SERVICE_H__
#include <stdbool.h>
#include <stdint.h>

#include "services/http/http_callback.h"

typedef enum
{
    HTTP_ERROR_NONE = 0,
    HTTP_ERROR_BUSY,
    HTTP_ERROR_TIMEOUT,
    HTTP_ERROR_AT_FAIL,
    HTTP_ERROR_BAD_STATUS
} http_error_t;

typedef struct
{
    char url[256];
    char payload[512];
    uint16_t payload_len;
    bool is_post;

    char user_agent[128];  // User-Agent header

    uint16_t http_status;
    uint16_t resp_len;
    uint16_t resp_received;

    char response[1024];
    uint16_t resp_pos;

    http_error_t error;
} http_context_t;

extern http_context_t http_ctx;
extern http_callback_t *http_cb;

void http_service_init(http_callback_t *cb);
bool http_get(const char *url);

#endif //__HTTP_SERVICE_H__