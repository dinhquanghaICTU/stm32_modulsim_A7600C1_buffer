#ifndef __MQTT_SERVICE_H__
#define __MQTT_SERVICE_H__

#include <stdbool.h>
#include <stdint.h>
#include "services/mqtt/mqtt_callback.h"

typedef enum{
    MQTT_ERROR_NONE = 0,
    MQTT_ERROR_BUSY,
    MQTT_ERROR_TIMEOUT,
    MQTT_ERROR_AT_FAIL,
}mqtt_error_t;

typedef struct{
    char broker_host[128];
    uint16_t broker_port;
    char client_id[64];
    bool is_connected;
    mqtt_error_t error;
}mqtt_context_t;

extern mqtt_context_t mqtt_ctx;
extern mqtt_callback_t *mqtt_cb;

void mqtt_service_init(mqtt_callback_t *cb);
bool mqtt_connect(const char *host, uint16_t port, const char *client_id);
bool mqtt_publish(const char *topic, const char *payload);
bool mqtt_subscribe(const char *topic);

#endif //__MQTT_SERVICE_H__