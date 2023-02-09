#ifndef SENSOR_H
#define SENSOR_H

#include "hardware/sensors/dht_sensor.h"
#include "hardware/sensors/stemma_soil_sensor.h"
#include "hardware/sensors/sonar_sensor.h"


typedef enum {
    SONAR_SENSOR            = 0,
    TEMP_HUMIDITY_SENSOR    = 1,
    MOISTURE_SENSOR         = 2
} SensorType;

typedef union {
    SonarSensor         mSonarSensor;
    DHT22Sensor         mDHTSensor;
    StemmaSoilSensor    mMoistureSensor;
} SensorHardware;

typedef enum {
    SENSOR_DISCONNECTED                 = 0x00,
    SENSOR_CONNECTED_MALFUNCTIONING     = 0x01,
    SENSOR_CONNECTED_NO_DATA            = 0x02,
    SENSOR_CONNECTED_VALID_DATA         = 0x03
} SensorStatus;

typedef union {
    uint16_t                mSonarSensorDistance;
    uint16_t                mMoistureSensorValue;
    DHT22Data               mTempHumidityData;
} SensorReading;

typedef struct {
    SensorStatus            mSensorStatus;
    SensorReading           mSensorReading;
} SensorData;

typedef struct {
    SensorHardware          mSensor;
    SensorType              mSensorType;
    uint8_t                 mSensorID;
    uint8_t                 mJackDetectPin;
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
void initialize_sensors(Sensor *sensors, uint8_t numSensors, i2c_inst_t *i2c, const int baud, const int sdaPin, const int sclPin);
void update_sensor_readings(Sensor *sensors, uint8_t numSensors);
void update_sensor_indicators(Sensor **sensors, uint8_t numSensors);

#endif      // SENSOR_H
