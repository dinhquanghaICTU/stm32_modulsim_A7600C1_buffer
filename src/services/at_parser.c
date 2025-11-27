#include "services/at_parser.h"
#include <string.h>
#include <stdlib.h>

bool at_parse_line(const char *line, at_event_t *evt)
{
    memset(evt, 0, sizeof(*evt));

    strncpy(evt->line, line, sizeof(evt->line));


    if (strcmp(line, "OK") == 0) {
        evt->type = AT_EVENT_OK;
        return true;
    }


    if (strcmp(line, "ERROR") == 0) {
        evt->type = AT_EVENT_ERROR;
        return true;
    }


    if (strcmp(line, "RDY") == 0) {
        evt->type = AT_EVENT_RDY;
        return true;
    }


    if (strncmp(line, "+CPIN:", 6) == 0) {
        evt->type = AT_EVENT_CPIN;
        return true;
    }


    if (strncmp(line, "+CREG:", 6) == 0) {
        evt->type = AT_EVENT_CREG;


        const char *p = strchr(line, ':');
        if (p) {
            evt->value1 = atoi(p + 1);
            const char *comma = strchr(p, ',');
            if (comma) evt->value2 = atoi(comma + 1);
        }

        return true;
    }


    if (strncmp(line, "+CSQ:", 5) == 0) {
        evt->type = AT_EVENT_CSQ;

        const char *p = strchr(line, ':');
        if (p) {
            evt->value1 = atoi(p + 1);        
            const char *comma = strchr(p, ',');
            if (comma) evt->value2 = atoi(comma + 1); 
        }

        return true;
    }


    if (strcmp(line, "SMS READY") == 0) {
        evt->type = AT_EVENT_SMS_READY;
        return true;
    }

    if (strcmp(line, "PB DONE") == 0) {
        evt->type = AT_EVENT_PBREADY;
        return true;
    }

    evt->type = AT_EVENT_URC;
    return true;
}
