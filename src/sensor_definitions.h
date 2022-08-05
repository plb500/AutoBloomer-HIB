#ifndef SENSOR_MSGPACK_IMPL_H
#define SENSOR_MSGPACK_IMPL_H

#include "hardware_definitions.h"
#include "hardware/sensors/sensor.h"
#include "hardware/sensors/sensor_msgpack.h"

// Sensor IDs/array positions
typedef enum {
    LOAD_SENSOR_L_ID        = 0,
    LOAD_SENSOR_R_ID        = 1,
    TEMP_SENSOR_L_ID        = 2,
    TEMP_SENSOR_R_ID        = 3,
    MOISTURE_SENSOR_L_ID    = 4,
    MOISTURE_SENSOR_R_ID    = 5,

    NUM_SENSORS
} SensorID;

extern Sensor LOAD_SENSOR_L_HW;
extern Sensor LOAD_SENSOR_R_HW;
extern Sensor TEMP_SENSOR_L_HW;
extern Sensor TEMP_SENSOR_R_HW;
extern Sensor MOISTURE_SENSOR_L_HW;
extern Sensor MOISTURE_SENSOR_R_HW;

// Our list of actual sensor hardware
extern Sensor* _sensorsList[NUM_SENSORS];

                        ///////////////////////////////////////////////////////
                        // Sensor output descriptions for connected hardware //
                        ///////////////////////////////////////////////////////

// Static reading definitions
extern MsgPackSensorReadingDescription MPACK_TEMPERATURE_READING_DESCRIPTION;
extern MsgPackSensorReadingDescription MPACK_HUMIDITY_READING_DESCRIPTION;
extern MsgPackSensorReadingDescription MPACK_SOIL_MOISTURE_READING_DESCRIPTION;
extern MsgPackSensorReadingDescription MPACK_WEIGHT_READING_DESCRIPTION;

// Reading output objects
extern MsgPackSensorReading MPACK_MOISTURE_SENSOR_L_READINGS[];
extern MsgPackSensorReading MPACK_MOISTURE_SENSOR_R_READINGS[];
extern MsgPackSensorReading MPACK_DHT22_SENSOR_L_READINGS[];
extern MsgPackSensorReading MPACK_DHT22_SENSOR_R_READINGS[];
extern MsgPackSensorReading MPACK_WEIGHT_SENSOR_L_READINGS[];
extern MsgPackSensorReading MPACK_WEIGHT_SENSOR_R_READINGS[];

                        /////////////////////////////////////////////////
                        // Sensor data wrappers for connected hardware //
                        /////////////////////////////////////////////////

extern MsgPackSensorData MPACK_MOISTURE_SENSOR_L;
extern MsgPackSensorData MPACK_MOISTURE_SENSOR_R;
extern MsgPackSensorData MPACK_DHT22_SENSOR_L;
extern MsgPackSensorData MPACK_DHT22_SENSOR_R;
extern MsgPackSensorData MPACK_WEIGHT_SENSOR_L;
extern MsgPackSensorData MPACK_WEIGHT_SENSOR_R;


// Container for the fake sensors/descriptions
extern MsgPackSensorData * SENSOR_MSGPACK[NUM_SENSORS];

#endif      // SENSOR_MSGPACK_IMPL_H
