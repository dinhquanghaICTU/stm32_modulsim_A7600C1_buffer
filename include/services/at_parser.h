#ifndef __AT_PARSER_H__
#define __AT_PARSER_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    AT_EVENT_NONE = 0,
    AT_EVENT_OK,
    AT_EVENT_ERROR,
    AT_EVENT_RDY,
    AT_EVENT_CPIN,
    AT_EVENT_CREG,
    AT_EVENT_CSQ,
    AT_EVENT_SMS_READY,
    AT_EVENT_PROMPT,
    AT_EVENT_CMGS,
    AT_EVENT_CMTI,
    AT_EVENT_CMGR_HEADER,
    AT_EVENT_CMGR_TEXT,
    AT_EVENT_CMS_ERROR,
    AT_EVENT_PBREADY,
    AT_EVENT_DOWNLOAD,
    AT_EVENT_HTTPACTION,
    AT_EVENT_HTTPREAD_HEADER,
    AT_EVENT_HTTPREAD_DATA,
    
    AT_EVENT_URC,     // unparsed URC
} at_event_type_t;

typedef struct {
    at_event_type_t type;
    char line[256];
    int value1;
    int value2;
} at_event_t;

bool at_parse_line(const char *line, at_event_t *evt);

#endif
