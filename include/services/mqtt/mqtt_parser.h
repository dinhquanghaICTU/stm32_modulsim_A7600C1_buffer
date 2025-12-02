#ifndef __MQTT_PARSER_H__
#define __MQTT_PARSER_H__

#include <stdbool.h>
#include "services/at_parser.h"

bool mqtt_parse_line(const char *line, at_event_t *evt);

#endif //__MQTT_PARSER_H__