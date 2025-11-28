#include "services/sms_recv/sms_recv_parser.h"
#include <string.h>
#include <stdlib.h>

bool sms_recv_parse_line(const char *line, at_event_t *evt)
{
    if (strncmp(line, "+CMTI:", 6) == 0)
    {
        evt->type = AT_EVENT_CMTI;
        evt->value1 = atoi(strrchr(line, ',') + 1);   
        return true;
    }

    if (strncmp(line, "+CMGR:", 6) == 0)
    {
        evt->type = AT_EVENT_CMGR_HEADER;
        strncpy(evt->line, line, sizeof(evt->line));
        return true;
    }

    if (line[0] != 0 && line[0] != '+')
    {
        evt->type = AT_EVENT_CMGR_TEXT;
        strncpy(evt->line, line, sizeof(evt->line));
        return true;
    }

    return false; 
}
