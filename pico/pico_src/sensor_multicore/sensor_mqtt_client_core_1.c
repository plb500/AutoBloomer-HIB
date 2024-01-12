#include "sensor_mqtt_client_core_1.h"

#include "sensor_multicore/sensor_multicore_utils.h"
#include "network/network_utils.h"
#include "network/mqtt_utils.h"
#include "debug_io.h"

#include "pico/time.h"


const uint16_t MQTT_UPDATE_PERIOD_MS = 5000;

extern queue_t sensorUpdateQueue;
MQTTState mqttState = {
    .mqttClient = 0,
    .mSensorUpdateQueue = &sensorUpdateQueue
};

DNSResolutionRequest brokerRequest = {
    .mResolvedAddress.addr = 0,
    .mHost = MQTT_BROKER
};

void sensor_mqtt_client_core_main() {
    connect_to_wifi(WIFI_SSID, WIFI_PASSWORD, PICO_HOSTNAME);
    
    init_mqtt_state(&mqttState);

    absolute_time_t timeout = nil_time;

    while(1) {
        if(!is_network_connected()) {
            DEBUG_PRINT("Network is not connected, connecting....\n");
            connect_to_wifi(WIFI_SSID, WIFI_PASSWORD, PICO_HOSTNAME);
        }

        absolute_time_t now = get_absolute_time();
        if(is_nil_time(timeout) || absolute_time_diff_us(now, timeout) <= 0) {
            if(is_network_connected()) {
                resolve_host_blocking(&brokerRequest);
            }

            if(brokerRequest.mResolvedAddress.addr) {
                mqttState.mBrokerAddress = brokerRequest.mResolvedAddress;
                mqttState.mBrokerPort = MQTT_PORT;

                if(!mqtt_client_is_connected(mqttState.mqttClient)) {
                    connect_to_broker_blocking(&mqttState);
                }

                if(mqtt_client_is_connected(mqttState.mqttClient)) {
                    pull_mqtt_data_from_queue(&mqttState);
                    timeout = make_timeout_time_ms(MQTT_UPDATE_PERIOD_MS);
                }
            }
        }
    }
}
