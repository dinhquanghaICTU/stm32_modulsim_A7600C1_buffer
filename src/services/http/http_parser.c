#include "services/http/http_parser.h"
#include <stdlib.h>
#include <string.h>

static bool httpread_body_pending = false;
static uint16_t httpread_remaining = 0;

bool http_parse_line(const char *line, at_event_t *evt)
{
    if (!line || !evt)
        return false;

    if (strncmp(line, "+HTTPACTION:", 12) == 0)
    {
        const char *p = line + 12;
        while (*p == ' ' || *p == '\t')
            ++p;

        int status = 0;
        int length = 0;

        const char *comma = strchr(p, ',');
        if (comma)
        {
            const char *status_str = comma + 1;
            status = atoi(status_str);

            const char *comma2 = strchr(status_str, ',');
            if (comma2)
                length = atoi(comma2 + 1);
        }

        evt->type = AT_EVENT_HTTPACTION;
        evt->value1 = status;
        evt->value2 = length;
        return true;
    }

    if (strncmp(line, "+HTTPREAD:", 10) == 0)
    {
        int length = atoi(line + 10);
        
        
        if (length == 0 && httpread_body_pending)
        {
        
            if (httpread_remaining == 0)
            {
                httpread_body_pending = false;
                return false;  
            }
            
            return false;
        }
    
        evt->type = AT_EVENT_HTTPREAD_HEADER;
        evt->value1 = length;
        httpread_body_pending = true;
        httpread_remaining = (length > 0) ? (uint16_t)length : 0;
        
        return true;
    }

    if (httpread_body_pending)
    {
        if (strcmp(line, "OK") == 0)
        {
            /* Kết thúc HTTPREAD: phát OK để FSM biết lần đọc đã xong */
            httpread_body_pending = false;
            httpread_remaining = 0;
            evt->type = AT_EVENT_OK;
            evt->value1 = 0;
            evt->value2 = 0;
            evt->line[0] = '\0';
            return true;
        }

        evt->type = AT_EVENT_HTTPREAD_DATA;
        
        
        size_t copy_len = httpread_remaining;
        if (copy_len > sizeof(evt->line) - 1)
            copy_len = sizeof(evt->line) - 1;
        
        
        if (copy_len == 0)
            copy_len = strlen(line);
        
        
        memcpy(evt->line, line, copy_len);
        evt->line[copy_len] = '\0';  
        evt->value1 = (int)copy_len;

        if (httpread_remaining > copy_len)
            httpread_remaining -= (uint16_t)copy_len;
        else
            httpread_remaining = 0;

        if (httpread_remaining == 0)
            httpread_body_pending = false;

        return true;
    }

    return false;
}

bool http_is_body_pending(void)
{
    return httpread_body_pending;
}

uint16_t http_get_body_remaining(void)
{
    return httpread_remaining;
}

void http_consume_body_bytes(uint16_t bytes)
{
    if (httpread_remaining > bytes)
        httpread_remaining -= bytes;
    else
        httpread_remaining = 0;
    
    if (httpread_remaining == 0)
        httpread_body_pending = false;
}

