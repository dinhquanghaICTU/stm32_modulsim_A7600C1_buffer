#ifndef __MQTT_CALLBACK_H__
#define __MQTT_CALLBACK_H__

typedef struct 
{
    // ham connect ket noi
    void (*on_mqtt_connected)(void);
    void (*on_mqtt_message)(const char *topic, const char *payload, int len);
    void (*on_mqtt_error)(int err);
}mqtt_callback_t;

#endif //__MQTT_CALLBACK_H__