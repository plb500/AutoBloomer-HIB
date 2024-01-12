#ifndef SENSOR_H
#define SENSOR_H

#include "sensor_i2c_interface.h"
#include "sonar_sensor.h"
#include "sensor_pod.h"
#include "battery_sensor.h"
#include "hardware/connected_hardware_monitor.h"


typedef enum {
    SONAR_SENSOR            = 0,
    SENSOR_POD              = 1,
    BATTERY_SENSOR          = 2
} SensorType;

typedef union {
    SonarSensor             mSonarSensor;
    SensorPod               mSensorPod;
    BatteryVoltageSensor    mBatterySensor;
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
    float                   mBatteryVoltage;
} SensorReading;

typedef struct {
    SensorStatus            mSensorStatus;
    SensorReading           mSensorReading;
} SensorData;

typedef struct {
    SensorHardware          mSensor;
    SensorType              mSensorType;
    uint8_t                 mSensorID;
    const char * const      mSensorName;
    const char * const      mSensorLocation;
    int8_t                  mSensorConnectLEDPosition;
    int8_t                  mHardwareConnectionID;
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

void update_sensors(Sensor *sensors, uint8_t numSensors, bool debugOutput, ConnectedHardwareMonitor *monitor);

#endif      // SENSOR_H
