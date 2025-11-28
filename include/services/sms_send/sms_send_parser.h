#ifndef __SMS_SEND_PARSER_H__
#define __SMS_SEND_PARSER_H__

#include <stdbool.h>
#include "services/at_parser.h"

// Trả về true nếu parse được, false nếu bỏ qua
bool sms_send_parse_line(const char *line, at_event_t *evt);

#endif
