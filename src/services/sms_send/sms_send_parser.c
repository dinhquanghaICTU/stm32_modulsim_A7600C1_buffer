#include "services/sms_send/sms_send_parser.h"
#include <string.h>
#include <stdlib.h>

bool sms_send_parse_line(const char *line, at_event_t *evt)
{
    if (line[0] == '>')
    {
        const char *p = line + 1;
        while (*p == ' ' || *p == '\t')
            ++p;

        if (*p == '\0')
        {
            evt->type = AT_EVENT_PROMPT;
            return true;
        }
    }

    if (strncmp(line, "+CMGS:", 6) == 0)
    {
        evt->type = AT_EVENT_CMGS;
        evt->value1 = atoi(line + 6);
        return true;
    }

    return false;
}
