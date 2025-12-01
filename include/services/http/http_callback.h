#ifndef __HTTP_CALLBACK_H__
#define __HTTP_CALLBACK_H__
#include "drivers/hardware.h"
#include <stdbool.h>
#include <string.h>

typedef struct
{
    void (*on_http_done)(int status, const char *body);
    void (*on_http_error)(int err);
} http_callback_t;

#endif //__HTTP_CALLBACK_H__