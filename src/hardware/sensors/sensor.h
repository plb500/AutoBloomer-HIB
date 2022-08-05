#ifndef SENSOR_H
#define SENSOR_H

#include "hardware/led_controller.h"
#include "hardware/sensors/dht_sensor.h"
#include "hardware/sensors/hx711.h"
#include "hardware/sensors/stemma_soil_sensor.h"


typedef enum {
    LOAD_SENSOR             = 0,
    TEMP_HUMIDITY_SENSOR    = 1,
    MOISTURE_SENSOR         = 2
} SensorType;

typedef union {
    float mLoadSensorWeight;
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
    HX711               mHX711;
    DHT22Sensor         mDHTSensor;
    StemmaSoilSensor    mMoistureSensor;
} SensorHardware;

typedef struct {
    SensorHardware      mSensor;
    SensorType          mSensorType;
    uint8_t             mSensorID;
    bool                mHardwareInitialized;
    uint8_t             mJackDetectPin;
} Sensor;

// Max reading values
static const float LOAD_SENSOR_MAX_VALUE        = 200.f;
static const float TEMP_SENSOR_MAX_VALUE        = 100.f;
static const float RH_SENSOR_MAX_VALUE          = 100.f;

bool is_sensor_connected(Sensor *sensor);
void initialize_sensors(Sensor **sensors, uint8_t numSensors, i2c_inst_t *i2c, const int baud, const int sdaPin, const int sclPin);
void update_sensor_readings(Sensor **sensors, SensorData *dataStorage, uint8_t numSensors);
void update_sensor_indicators(Sensor **sensors, uint8_t numSensors);

#endif      // SENSOR_H
