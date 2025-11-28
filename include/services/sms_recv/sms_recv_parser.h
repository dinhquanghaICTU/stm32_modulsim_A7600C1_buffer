#ifndef __SMS_RECV_PARSER_H__
#define __SMS_RECV_PARSER_H__

#include <stdbool.h>
#include "services/at_parser.h"


bool sms_recv_parse_line(const char *line, at_event_t *evt);

#endif /* __SMS_RECV_PARSER_H__ */
