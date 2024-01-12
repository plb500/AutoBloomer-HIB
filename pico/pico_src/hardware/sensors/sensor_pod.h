#ifndef _SENSOR_POD_H_
#define _SENSOR_POD_H_

#include "sensor_i2c_interface.h"


#define SCD30_I2C_ADDRESS                       (0x61)

typedef enum {
    SOIL_SENSOR_1_ADDRESS = 0x36,
    SOIL_SENSOR_2_ADDRESS = 0x37,
    SOIL_SENSOR_3_ADDRESS = 0x38,
    SOIL_SENSOR_4_ADDRESS = 0x39
} SoilSensorAddresses;

typedef struct {
    bool mSCD30SensorDataValid;
    bool mSoilSensorDataValid;

    float mCO2Level;
    float mTemperature;
    float mHumidity;
    uint16_t mSoilSensorData;
} SensorPodData;

typedef struct {
    I2CInterface *mInterface;
    I2CChannel mI2CChannel;
    uint8_t mSCD30Address;
    uint8_t mSoilSensorAddress;
    bool mSoilSensorActive;
    bool mSCD30SensorActive;
    SensorPodData mCurrentData;
    absolute_time_t mPodResetTimeout;
} SensorPod;


bool initialize_sensor_pod(SensorPod *sensorPod);
bool reset_sensor_pod(SensorPod *sensorPod);
void update_sensor_pod(SensorPod *sensorPod);
bool sensor_pod_has_valid_data(SensorPod *sensorPod);



#endif
