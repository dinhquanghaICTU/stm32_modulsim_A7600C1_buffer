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
            httpread_body_pending = false;
            httpread_remaining = 0;
            return false;
        }

        evt->type = AT_EVENT_HTTPREAD_DATA;
        strncpy(evt->line, line, sizeof(evt->line) - 1);
        evt->line[sizeof(evt->line) - 1] = '\0';

        size_t chunk_len = strlen(evt->line);
        if (httpread_remaining > chunk_len)
            httpread_remaining -= (uint16_t)chunk_len;
        else
            httpread_remaining = 0;

        if (httpread_remaining == 0)
            httpread_body_pending = false;

        return true;
    }

    return false;
}

