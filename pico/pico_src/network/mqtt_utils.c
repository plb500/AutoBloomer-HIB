#include "mqtt_utils.h"

#include "debug_io.h"

#include <string.h>
#include "lwip/pbuf.h"
#include "lwip/apps/mqtt_priv.h"
#include "pico/cyw43_arch.h"


typedef struct ConnectMonitor_t {
    mqtt_connection_status_t mStatus;
    bool mConnectionCompleted;
} ConnectionMonitor;

void init_mqtt_state(MQTTState *mqttState) {
    if(!mqttState) {
        return;
    }

    if(mqttState->mqttClient) {
        return;
    }

    mqttState->mqttClient = mqtt_client_new();
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    ConnectionMonitor *monitor = (ConnectionMonitor*) arg;
    monitor->mStatus = status;
    monitor->mConnectionCompleted = true;
}

void connect_to_broker_blocking(MQTTState *mqttState) {
    struct mqtt_connect_client_info_t ci;
    err_t err;

    if(!mqttState) {
        return;
    }

    memset(&ci, 0, sizeof(ci));
    ci.client_id    = "SensorPod";
    ci.client_user  = NULL;
    ci.client_pass  = NULL;
    ci.keep_alive   = 10;
    ci.will_topic   = NULL;
    ci.will_msg     = NULL;
    ci.will_retain  = 0;
    ci.will_qos     = 0;

    ConnectionMonitor connectionMonitor;
    connectionMonitor.mConnectionCompleted = false;

    mqtt_client_connect(mqttState->mqttClient, &mqttState->mBrokerAddress, mqttState->mBrokerPort, mqtt_connection_cb, &connectionMonitor, &ci);

    while(!connectionMonitor.mConnectionCompleted) {
        sleep_ms(1);
    }
}

void disconnect_from_broker(MQTTState *mqttState) {
    if(!mqttState) {
        return;
    }

    if(mqtt_client_is_connected(mqttState->mqttClient)) {
        mqtt_disconnect(mqttState->mqttClient);
    }
}

void mqtt_pub_request_cb(void *arg, err_t err) {}

err_t publish_mqtt_data(MQTTState *mqttState, const char *topic, const char *payload) {
    err_t err;
    u8_t qos = 0;
    u8_t retain = 0;
  
    err = mqtt_publish(mqttState->mqttClient, topic, payload, strlen(payload), qos, retain, mqtt_pub_request_cb, mqttState);

    if(err != ERR_OK) {
        DEBUG_PRINT("Publish err: %d\n", err);
    }

    return err; 
}

void init_mqtt_message(MQTTMessage *message) {
    if(message) {
        memset(message->mTopic, 0, MQTT_MAX_PAYLOAD_LENGTH);
        memset(message->mPayload, 0, MQTT_MAX_PAYLOAD_LENGTH);
    }
}

err_t publish_mqtt_message(MQTTState *mqttState, MQTTMessage *message) {
    if(message && mqttState) {
        return publish_mqtt_data(mqttState, message->mTopic, message->mPayload);
    }

    return ERR_ARG;
}
