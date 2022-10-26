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
    uint16_t mSonarSensorDistance;
    uint16_t mMoistureSensorValue;
    DHT22Data mTempHumidityData;
} SensorReading;

typedef struct {
    uint8_t mSensorID;
    SensorReading mSensorReading;
    SensorType mType;
    bool mIsSensorReadingValid;
    bool mIsSensorConnected;
} SensorData;

typedef union {
    SonarSensor         mSonarSensor;
    DHT22Sensor         mDHTSensor;
    StemmaSoilSensor    mMoistureSensor;
} SensorHardware;

typedef struct {
    SensorHardware      mSensor;
    SensorType          mSensorType;
    uint8_t             mSensorID;
    bool                mHardwareInitialized;
    uint8_t             mJackDetectPin;
    uint8_t             mSensorConnectLEDPosition;
} Sensor;

// Max reading values
static const uint16_t SONAR_SENSOR_MAX_VALUE    = 16000;
static const float TEMP_SENSOR_MAX_VALUE        = 100.f;
static const float RH_SENSOR_MAX_VALUE          = 100.f;

bool is_sensor_connected(Sensor *sensor);
void initialize_sensors(Sensor **sensors, uint8_t numSensors, i2c_inst_t *i2c, const int baud, const int sdaPin, const int sclPin);
void update_sensor_readings(Sensor **sensors, SensorData *dataStorage, uint8_t numSensors);
void update_sensor_indicators(Sensor **sensors, uint8_t numSensors);

#endif      // SENSOR_H
