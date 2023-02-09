#ifndef SENSOR_MULTICORE_UTILS_H
#define SENSOR_MULTICORE_UTILS_H

#include "hardware/sensors/sensor.h"
#include "hardware/sensors/sensor_msgpack.h"
#include "pico/util/queue.h"


typedef struct {
    uint8_t                 mSensorID;
    SensorData              mSensorData;
} SensorDataUpdateMessage;

void sensor_to_update_message(Sensor *sensor, SensorDataUpdateMessage *updateMessage);
void update_message_to_sensor_packet(SensorDataUpdateMessage *updateMessage, MsgPackSensorPacket *sensorPacket);

void intitialize_sensor_data_queue(queue_t *sensorDataQueue, int numMessages);
void push_sensor_data_to_queue(queue_t *sensorDataQueue, Sensor *sensor);


#endif
