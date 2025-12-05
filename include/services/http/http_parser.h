#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

#include <stdbool.h>
#include <stdint.h>
#include "services/at_parser.h"

bool http_parse_line(const char *line, at_event_t *evt);
bool http_is_body_pending(void);
uint16_t http_get_body_remaining(void);
void http_consume_body_bytes(uint16_t bytes);

#endif // __HTTP_PARSER_H__

