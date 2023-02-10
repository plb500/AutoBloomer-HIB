#ifndef SENSOR_DEFINITIONS_H
#define SENSOR_DEFINITIONS_H

#include "hardware_definitions.h"
#include "hardware/sensors/sensor.h"
#include "uart_controller/sensor_msgpack.h"

// Sensor IDs/array positions
typedef enum {
    SONAR_SENSOR_L_ID       = 0,
    SONAR_SENSOR_R_ID       = 1,
    TEMP_SENSOR_L_ID        = 2,
    TEMP_SENSOR_R_ID        = 3,
    MOISTURE_SENSOR_L_ID    = 4,
    MOISTURE_SENSOR_R_ID    = 5,

    NUM_SENSORS
} SensorID;

// Our list of actual sensor hardware
extern Sensor sensorsList[NUM_SENSORS];

// Data transmission wrappers for connected hardware 
extern MsgPackSensorPacket sensorPackets[NUM_SENSORS];

#endif      // SENSOR_DEFINITIONS_H
