#ifndef SENSOR_H
#define SENSOR_H

#include "hardware/sensors/sensor_i2c_interface.h"
#include "hardware/sensors/sonar_sensor.h"
#include "hardware/sensors/sensor_pod.h"


typedef enum {
    SONAR_SENSOR            = 0,
    SENSOR_POD              = 1
} SensorType;

typedef union {
    SonarSensor         mSonarSensor;
    SensorPod           mSensorPod;
} SensorHardware;

typedef enum {
    SENSOR_DISCONNECTED                 = 0x00,
    SENSOR_CONNECTED_MALFUNCTIONING     = 0x01,
    SENSOR_CONNECTED_NO_DATA            = 0x02,
    SENSOR_CONNECTED_VALID_DATA         = 0x03
} SensorStatus;

typedef union {
    uint16_t                mSonarSensorDistance;
    SensorPodData           mSensorPodData;
} SensorReading;

typedef struct {
    SensorStatus            mSensorStatus;
    SensorReading           mSensorReading;
} SensorData;

typedef struct {
    SensorHardware          mSensor;
    SensorType              mSensorType;
    uint8_t                 mSensorID;
    uint8_t                 mSensorConnectLEDPosition;
} SensorDefinition;

typedef struct {
    SensorDefinition        mSensorDefinition;
    bool                    mHardwareInitialized;
    SensorData              mCurrentSensorData;
} Sensor;

// Max reading values
static const uint16_t SONAR_SENSOR_MAX_VALUE    = 16000;
static const float TEMP_SENSOR_MAX_VALUE        = 100.f;
static const float RH_SENSOR_MAX_VALUE          = 100.f;

bool is_sensor_connected(Sensor *sensor);
void initialize_sensors(Sensor *sensors, uint8_t numSensors, I2CInterface *i2cInterface);
void update_sensor_readings(Sensor *sensors, uint8_t numSensors);
void update_sensor_indicators(Sensor **sensors, uint8_t numSensors);

#endif      // SENSOR_H
