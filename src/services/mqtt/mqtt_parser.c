#include "services/mqtt/mqtt_parser.h"

#include <stdlib.h>
#include <string.h>

bool mqtt_parse_line(const char *line, at_event_t *evt)
{
    if (!line || !evt)
        return false;

    // +CMQTTCONNECT: 0
    if (strncmp(line, "+CMQTTCONNECT:", 14) == 0)
    {
        int result = atoi(line + 14);
        evt->type = AT_EVENT_MQTTCONNECT;
        evt->value1 = result;
        return true;
    }

    // +CMQTTCONNACK: 0,1
    // Format: +CMQTTCONNACK: <return_code>,<session_present>
    if (strncmp(line, "+CMQTTCONNACK:", 14) == 0)
    {
        const char *p = line + 14;
        while (*p == ' ' || *p == '\t')
            ++p;

        int return_code = atoi(p);
        
        const char *comma = strchr(p, ',');
        int session_present = 0;
        if (comma)
        {
            session_present = atoi(comma + 1);
        }

        evt->type = AT_EVENT_MQTTCONNACK;
        evt->value1 = return_code;      // 0 = success, >0 = error code
        evt->value2 = session_present;  // 0 or 1
        return true;
    }


    if (strncmp(line, "+CMQTTPUBLISH:", 14) == 0)
    {
        const char *p = line + 14;
        while (*p == ' ' || *p == '\t')
            ++p;

        
        int msg_id = atoi(p);
        
        
        const char *topic_start = strchr(p, '"');
        if (!topic_start)
            return false;
        
        topic_start++;  
        
        
        const char *topic_end = strchr(topic_start, '"');
        if (!topic_end)
            return false;
        
        
        size_t topic_len = topic_end - topic_start;
        if (topic_len >= sizeof(evt->line))
            topic_len = sizeof(evt->line) - 1;
        
        memcpy(evt->line, topic_start, topic_len);
        evt->line[topic_len] = '\0';
        
        
        const char *comma = strchr(topic_end, ',');
        if (!comma)
            return false;
        
        
        int payload_len = atoi(comma + 1);
        
        
        const char *payload_start = strchr(comma, '"');
        if (!payload_start)
            return false;
        
        payload_start++;  
        
        
        const char *payload_end = strchr(payload_start, '"');
        if (!payload_end)
            return false;
        
        // Extract payload (store after topic, separated by \0)
        size_t payload_size = payload_end - payload_start;
        size_t max_payload = sizeof(evt->line) - topic_len - 2;
        if (payload_size > max_payload)
            payload_size = max_payload;
        
        // Store payload after topic (with separator)
        char *payload_dest = evt->line + topic_len + 1;
        memcpy(payload_dest, payload_start, payload_size);
        payload_dest[payload_size] = '\0';
        
        evt->type = AT_EVENT_MQTTPUBLISH;
        evt->value1 = msg_id;
        evt->value2 = payload_len;
        return true;
    }

    // +CMQTTPUBACK: 0,1
    // Format: +CMQTTPUBACK: <msg_id>,<result>
    if (strncmp(line, "+CMQTTPUBACK:", 13) == 0)
    {
        const char *p = line + 13;
        while (*p == ' ' || *p == '\t')
            ++p;

        int msg_id = atoi(p);
        
        const char *comma = strchr(p, ',');
        int result = 0;
        if (comma)
        {
            result = atoi(comma + 1);
        }

        evt->type = AT_EVENT_MQTTPUBACK;
        evt->value1 = msg_id;
        evt->value2 = result;  // 0 = success
        return true;
    }

    // +CMQTTSUBACK: 0,1,1
    // Format: +CMQTTSUBACK: <msg_id>,<result>,<qos>
    if (strncmp(line, "+CMQTTSUBACK:", 13) == 0)
    {
        const char *p = line + 13;
        while (*p == ' ' || *p == '\t')
            ++p;

        int msg_id = atoi(p);
        
        const char *comma1 = strchr(p, ',');
        int result = 0;
        int qos = 0;
        
        if (comma1)
        {
            result = atoi(comma1 + 1);
            
            const char *comma2 = strchr(comma1 + 1, ',');
            if (comma2)
            {
                qos = atoi(comma2 + 1);
            }
        }

        evt->type = AT_EVENT_MQTTSUBACK;
        evt->value1 = msg_id;
        evt->value2 = result;  // 0 = success, >0 = error
        // Note: qos có thể lưu vào value2 nếu cần, hoặc parse từ topic
        return true;
    }

    // +CMQTTDISC: 0
    if (strncmp(line, "+CMQTTDISC:", 11) == 0)
    {
        int result = atoi(line + 11);
        evt->type = AT_EVENT_MQTTDISC;
        evt->value1 = result;
        return true;
    }

    return false;
}

