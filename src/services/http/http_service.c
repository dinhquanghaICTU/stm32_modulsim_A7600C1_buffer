#include "services/http/http_service.h"

#include <string.h>

#include "fsm/http_fsm.h"

http_context_t http_ctx;
http_callback_t *http_cb = NULL;

void http_service_init(http_callback_t *cb)
{
    memset(&http_ctx, 0, sizeof(http_ctx));
    http_cb = cb;
    http_fsm_init();
}

bool http_get(const char *url)
{
    if (!url || url[0] == '\0')
        return false;

    if (http_get_state() != HTTP_IDLE)
    {
        http_ctx.error = HTTP_ERROR_BUSY;
        return false;
    }

    memset(&http_ctx, 0, sizeof(http_ctx));
    strncpy(http_ctx.url, url, sizeof(http_ctx.url) - 1);
    http_ctx.url[sizeof(http_ctx.url) - 1] = '\0';
    http_ctx.is_post = false;
    http_ctx.error = HTTP_ERROR_NONE;
    http_set_state(HTTP_REQ_START);
    return true;
}
