#ifndef _SCD30_SENSOR_H_
#define _SCD30_SENSOR_H_

#include "sensor_i2c_interface.h"


extern const uint16_t SCD30_SERIAL_BYTE_SIZE;


typedef struct {
    bool mValidReading;
    float mCO2Reading;
    float mTemperatureReading;
    float mHumidityReading;
} SCD30SensorData;


bool trigger_scd30_continuous_measurement(I2CInterface *i2cInterface, uint8_t address, uint16_t pressureCompensation);
bool stop_scd30_continuous_measurement(I2CInterface *i2cInterface, uint8_t address);
bool set_scd30_measurement_interval(I2CInterface *i2cInterface, uint8_t address, uint16_t measurementInterval);
bool get_scd30_data_ready_status(I2CInterface *i2cInterface, uint8_t address);
SCD30SensorData get_scd30_reading(I2CInterface *i2cInterface, uint8_t address);
bool set_scd30_automatic_self_calibration(I2CInterface *i2cInterface, uint8_t address, bool selfCalibrationOn);
bool set_scd30_forced_recalibration_value(I2CInterface *i2cInterface, uint8_t address, uint16_t referenceValue);
bool set_scd30_temperature_offset(I2CInterface *i2cInterface, uint8_t address, uint16_t temperatureOffset);
bool set_scd30_altitude_compensation(I2CInterface *i2cInterface, uint8_t address, uint16_t altitude);
bool read_scd30_firmware_version(I2CInterface *i2cInterface, uint8_t address, uint8_t *dst);
bool do_scd30_soft_reset(I2CInterface *i2cInterface, uint8_t address);
bool read_scd30_serial(I2CInterface *i2cInterface, uint8_t address, char *dst);



#endif
