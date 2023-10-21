#ifndef SENSOR_MULTICORE_UTILS_H
#define SENSOR_MULTICORE_UTILS_H

#include "hardware/sensors/sensor.h"
#include "sensor_definitions.h"
#include "uart_controller/sensor_msgpack.h"
#include "pico/util/queue.h"


// Queue management
void intitialize_sensor_data_queue(queue_t *sensorDataQueue, int numMessages);
void push_sensor_data_to_queue(queue_t *sensorDataQueue, Sensor *sensors);
void consume_update_queue_messages(queue_t *sensorUpdateQueue, MsgPackSensorPacket *sensorPackets);


#endif
