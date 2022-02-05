#ifndef STEMMA_SOIL_SENSOR_H
#define STEMMA_SOIL_SENSOR_H

#include "hardware/i2c.h"

typedef enum {
    SOIL_SENSOR_1_ADDRESS = 0x36,
    SOIL_SENSOR_2_ADDRESS = 0x37,
    SOIL_SENSOR_3_ADDRESS = 0x38,
    SOIL_SENSOR_4_ADDRESS = 0x39
} SoilSensorAddresses;

typedef struct {
    i2c_inst_t *mI2C;
    uint mAddress;
} StemmaSoilSensor;

void init_sensor_bus(i2c_inst_t *i2c, const int sdaPin, const int sclPin);
bool soil_sensor_begin(StemmaSoilSensor *soilSensor);
bool reset_soil_sensor(StemmaSoilSensor *soilSensor);
uint get_soil_sensor_version(StemmaSoilSensor *soilSensor);
uint16_t get_soil_sensor_capacitive_value(StemmaSoilSensor *soilSensor);

#endif  // STEMMA_SOIL_SENSOR_H
