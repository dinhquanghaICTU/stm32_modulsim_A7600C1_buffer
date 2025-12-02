#include "fsm/mqtt_fsm.h"
#include <string.h>

#include "services/uart_channel.h"
#include "drivers/hardware.h"
#include "services/at_parser.h"
#include "services/mqtt/mqtt_service.h"

extern mqtt_context_t mqtt_ctx;
extern mqtt_callback_t *mqtt_cb;

static mqtt_state_t mqtt_state = MQTT_IDLE;
static uint32_t state_timestamp = 0;

static char pending_topic[128];
static char pending_payload[512];
static uint8_t pending_qos = 0;
static uint16_t pending_msg_id = 0;

#define MQTT_CMD_TIMEOUT_MS      5000U
#define MQTT_CONNECT_TIMEOUT_MS 15000U

static void mqtt_fail(mqtt_error_t err)
{
    mqtt_ctx.error = err;
    mqtt_set_state(MQTT_ERROR);
}

mqtt_state_t mqtt_get_state(void)
{
    return mqtt_state;
}

void mqtt_fsm_init(void)
{
    mqtt_state = MQTT_IDLE;
    state_timestamp = HW_GetTickMs();
    pending_topic[0] = '\0';
    pending_payload[0] = '\0';
    pending_qos = 0;
    pending_msg_id = 0;
}

void mqtt_set_state(mqtt_state_t st)
{
    mqtt_state = st;
    state_timestamp = HW_GetTickMs();

    switch (st)
    {
    case MQTT_IDLE:
        break;

    case MQTT_CONNECT_REQ:
        
        mqtt_set_state(MQTT_CONNECTING);
        break;

    case MQTT_INIT:
        
        mqtt_set_state(MQTT_CONNECTING);
        break;

    case MQTT_WAIT_INIT:
        
        mqtt_set_state(MQTT_CONNECTING);
        break;

    case MQTT_CONNECTING:
        
        uart_channel_send_format(UART_CH_SIM,"AT+CMQTTCONNECT=0,\"%s\",%u,\"%s\"\r\n",mqtt_ctx.broker_host,mqtt_ctx.broker_port,mqtt_ctx.client_id);
        mqtt_state = MQTT_WAIT_CONNACK;
        state_timestamp = HW_GetTickMs();
        break;

    case MQTT_WAIT_CONNACK:
        break;

    case MQTT_CONNECTED:
        mqtt_ctx.is_connected = true;
        if (mqtt_cb && mqtt_cb->on_mqtt_connected)
            mqtt_cb->on_mqtt_connected();
        break;

    case MQTT_DISCONNECT_REQ:
        break;

    case MQTT_DISCONNECTING:
        uart_channel_send_str(UART_CH_SIM, "AT+CMQTTDISC=0\r\n");
        mqtt_state = MQTT_DISCONNECTED;
        state_timestamp = HW_GetTickMs();
        break;

    case MQTT_DISCONNECTED:
        mqtt_ctx.is_connected = false;
        mqtt_state = MQTT_IDLE;
        break;

    case MQTT_PUBLISH_REQ:
        break;

    case MQTT_PUBLISHING:
        
        uart_channel_send_format(UART_CH_SIM,"AT+CMQTTTOPIC=0,%u\r\n",(unsigned int)strlen(pending_topic));

        uart_channel_send_str(UART_CH_SIM, pending_topic);
        HW_DelayMs(100);

        uart_channel_send_format(UART_CH_SIM, "AT+CMQTTPAYLOAD=0,%u\r\n",(unsigned int)strlen(pending_payload));

        uart_channel_send_str(UART_CH_SIM, pending_payload);
        HW_DelayMs(100);
        uart_channel_send_format(UART_CH_SIM,"AT+CMQTTPUB=0,0,0,%u\r\n",pending_msg_id);
        mqtt_state = MQTT_WAIT_PUBACK;
        state_timestamp = HW_GetTickMs();
        break;

    case MQTT_WAIT_PUBACK:
        break;

    case MQTT_SUBSCRIBE_REQ:
        break;

    case MQTT_SUBSCRIBING:
        uart_channel_send_format(UART_CH_SIM,"AT+CMQTTSUBTOPIC=0,%u\r\n",(unsigned int)strlen(pending_topic));
        uart_channel_send_str(UART_CH_SIM, pending_topic);
        HW_DelayMs(100);
        uart_channel_send_format(UART_CH_SIM,"AT+CMQTTSUB=0,%u,%u\r\n",pending_qos,pending_msg_id);
        mqtt_state = MQTT_WAIT_SUBACK;
        state_timestamp = HW_GetTickMs();
        break;

    case MQTT_WAIT_SUBACK:
        break;

    case MQTT_WAIT_MESSAGE:
        break;

    case MQTT_ERROR:
        mqtt_ctx.is_connected = false;
        if (mqtt_cb && mqtt_cb->on_mqtt_error)
            mqtt_cb->on_mqtt_error(mqtt_ctx.error);
        mqtt_state = MQTT_IDLE;
        break;
    }
}

void mqtt_fsm_tick(event_queue_t *q)
{
    at_event_t evt;

    switch (mqtt_state)
    {
    case MQTT_IDLE:
        event_queue_pop(q, &evt);
        break;

    case MQTT_CONNECT_REQ:
        mqtt_set_state(MQTT_CONNECTING);
        break;

    case MQTT_INIT:
        
        mqtt_set_state(MQTT_CONNECTING);
        break;

    case MQTT_WAIT_INIT:
        
        mqtt_set_state(MQTT_CONNECTING);
        break;

    case MQTT_CONNECTING:
        
        break;

    case MQTT_WAIT_CONNACK:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_MQTTCONNACK)
            {
                if (evt.value1 == 0)  
                {
                    mqtt_set_state(MQTT_CONNECTED);
                }
                else
                {
                    mqtt_ctx.error = MQTT_ERROR_AT_FAIL;
                    mqtt_fail(MQTT_ERROR_AT_FAIL);
                }
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                mqtt_fail(MQTT_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, MQTT_CONNECT_TIMEOUT_MS))
        {
            mqtt_fail(MQTT_ERROR_TIMEOUT);
        }
        break;

    case MQTT_CONNECTED:
        
        if (event_queue_pop(q, &evt))
        {
            
            if (evt.type == AT_EVENT_MQTTPUBLISH)
            {
                
                const char *topic = evt.line;
                const char *payload = evt.line + strlen(evt.line) + 1;
                int payload_len = evt.value2;

                if (mqtt_cb && mqtt_cb->on_mqtt_message)
                {
                    mqtt_cb->on_mqtt_message(topic, payload, payload_len);
                }
            }
            
            else if (evt.type == AT_EVENT_MQTTPUBACK)
            {
                
                mqtt_state = MQTT_CONNECTED;
            }
            // Handle subscribe ACK
            else if (evt.type == AT_EVENT_MQTTSUBACK)
            {
                if (evt.value2 == 0)  // success
                {
                    // Subscribe successful
                }
                // Return to connected state
                mqtt_state = MQTT_CONNECTED;
            }
            // Handle disconnect
            else if (evt.type == AT_EVENT_MQTTDISC)
            {
                mqtt_set_state(MQTT_DISCONNECTED);
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                mqtt_fail(MQTT_ERROR_AT_FAIL);
            }
        }
        break;

    case MQTT_DISCONNECT_REQ:
        mqtt_set_state(MQTT_DISCONNECTING);
        break;

    case MQTT_DISCONNECTING:
        // State transition handled in mqtt_set_state
        break;

    case MQTT_DISCONNECTED:
        // Already handled in mqtt_set_state
        break;

    case MQTT_PUBLISH_REQ:
        mqtt_set_state(MQTT_PUBLISHING);
        break;

    case MQTT_PUBLISHING:
        // State transition handled in mqtt_set_state
        break;

    case MQTT_WAIT_PUBACK:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_MQTTPUBACK)
            {
                
                mqtt_set_state(MQTT_CONNECTED);
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                mqtt_fail(MQTT_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, MQTT_CMD_TIMEOUT_MS))
        {
            mqtt_fail(MQTT_ERROR_TIMEOUT);
        }
        break;

    case MQTT_SUBSCRIBE_REQ:
        mqtt_set_state(MQTT_SUBSCRIBING);
        break;

    case MQTT_SUBSCRIBING:
        
        break;

    case MQTT_WAIT_SUBACK:
        if (event_queue_pop(q, &evt))
        {
            if (evt.type == AT_EVENT_MQTTSUBACK)
            {
                if (evt.value2 == 0)  
                {
                    
                }
                mqtt_set_state(MQTT_CONNECTED);
            }
            else if (evt.type == AT_EVENT_ERROR)
            {
                mqtt_fail(MQTT_ERROR_AT_FAIL);
            }
        }
        else if (HW_IsTimeout(&state_timestamp, MQTT_CMD_TIMEOUT_MS))
        {
            mqtt_fail(MQTT_ERROR_TIMEOUT);
        }
        break;

    case MQTT_WAIT_MESSAGE:
        
        break;

    case MQTT_ERROR:
        break;
    }
}


void mqtt_set_publish_data(const char *topic, const char *payload)
{
    strncpy(pending_topic, topic, sizeof(pending_topic) - 1);
    pending_topic[sizeof(pending_topic) - 1] = '\0';
    
    strncpy(pending_payload, payload, sizeof(pending_payload) - 1);
    pending_payload[sizeof(pending_payload) - 1] = '\0';
}

void mqtt_set_subscribe_data(const char *topic, uint8_t qos)
{
    strncpy(pending_topic, topic, sizeof(pending_topic) - 1);
    pending_topic[sizeof(pending_topic) - 1] = '\0';
    pending_qos = qos;
}
