#ifndef SENSOR_DEFINITIONS_H
#define SENSOR_DEFINITIONS_H

#include "hardware_definitions.h"
#include "hardware/sensors/sensor.h"
#include "uart_controller/sensor_msgpack.h"

// Sensor IDs/array positions
typedef enum {
    SONAR_SENSOR_L1_ID      = 0,
    SONAR_SENSOR_R1_ID      = 1,
    SENSOR_POD_L_ID         = 2,
    SENSOR_POD_R_ID         = 3,

    NUM_SENSORS
} SensorID;

// Sensor I2C bus interfaces
extern I2CInterface sensorI2CInterface;

// Our list of actual sensor hardware - used on core0 only
extern Sensor sensorsList[NUM_SENSORS];

// Data transmission wrappers for connected hardware - used on core1 only
extern MsgPackSensorPacket sensorPackets[NUM_SENSORS];

#endif      // SENSOR_DEFINITIONS_H
