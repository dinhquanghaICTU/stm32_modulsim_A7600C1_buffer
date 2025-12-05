#include "services/http/http_service.h"
#include <string.h>
#include "fsm/http_fsm.h"

http_context_t http_ctx;
http_callback_t *http_cb = NULL;

static const char* default_user_agent = "A7600C1_HTTP/1.0";

void http_service_init(http_callback_t *cb)
{
    memset(&http_ctx, 0, sizeof(http_ctx));
    http_cb = cb;
    http_fsm_init();
}

void http_set_user_agent(const char *user_agent)
{
    if (user_agent && user_agent[0] != '\0')
    {
        strncpy(http_ctx.user_agent, user_agent, sizeof(http_ctx.user_agent) - 1);
        http_ctx.user_agent[sizeof(http_ctx.user_agent) - 1] = '\0';
    }
    else
    {
        http_ctx.user_agent[0] = '\0';
    }
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

    char saved_ua[128];
    bool has_custom_ua = (http_ctx.user_agent[0] != '\0');
    if (has_custom_ua)
    {
        strncpy(saved_ua, http_ctx.user_agent, sizeof(saved_ua) - 1);
        saved_ua[sizeof(saved_ua) - 1] = '\0';
    }

    memset(&http_ctx, 0, sizeof(http_ctx));
    strncpy(http_ctx.url, url, sizeof(http_ctx.url) - 1);
    http_ctx.url[sizeof(http_ctx.url) - 1] = '\0';
    http_ctx.is_post = false;
    http_ctx.error = HTTP_ERROR_NONE;
    
    if (has_custom_ua)
    {
        strncpy(http_ctx.user_agent, saved_ua, sizeof(http_ctx.user_agent) - 1);
        http_ctx.user_agent[sizeof(http_ctx.user_agent) - 1] = '\0';
    }
    else
    {
        strncpy(http_ctx.user_agent, default_user_agent, sizeof(http_ctx.user_agent) - 1);
        http_ctx.user_agent[sizeof(http_ctx.user_agent) - 1] = '\0';
    }
    
    http_set_state(HTTP_REQ_START);
    return true;
}

