#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

#include <stdbool.h>

#include "services/at_parser.h"

bool http_parse_line(const char *line, at_event_t *evt);

#endif // __HTTP_PARSER_H__

