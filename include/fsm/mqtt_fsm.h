#ifndef __MQTT_FSM_H__
#define __MQTT_FSM_H__

#include "services/event_queue.h"
#include "drivers/hardware.h"
#include "services/mqtt/mqtt_service.h"
#include <stdint.h>

typedef enum{
    
    MQTT_IDLE = 0,
    MQTT_CONNECT_REQ,
    MQTT_INIT,          
    MQTT_WAIT_INIT,
    MQTT_CONNECTING,
    MQTT_WAIT_CONNACK,
    MQTT_CONNECTED,
    MQTT_DISCONNECT_REQ,
    MQTT_DISCONNECTING,
    MQTT_DISCONNECTED,
    
    
    MQTT_PUBLISH_REQ,
    MQTT_PUBLISHING,
    MQTT_WAIT_PUBACK,
    
    
    MQTT_SUBSCRIBE_REQ,
    MQTT_SUBSCRIBING,
    MQTT_WAIT_SUBACK,
    
    
    MQTT_WAIT_MESSAGE,
    
    
    MQTT_ERROR

}mqtt_state_t;

void mqtt_fsm_init(void);
void mqtt_fsm_tick(event_queue_t *q);
void mqtt_set_state(mqtt_state_t st);
mqtt_state_t mqtt_get_state(void);
void mqtt_set_publish_data(const char *topic, const char *payload);
void mqtt_set_subscribe_data(const char *topic, uint8_t qos);

#endif //__MQTT_FSM_H__