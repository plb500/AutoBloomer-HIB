#include "sensor_pod.h"

#include "stemma_soil_sensor.h"
#include "scd30_sensor.h"
#include "debug_io.h"

I2CResponse select_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return false;
    }

    return select_i2c_channel(sensorPod->mInterface, sensorPod->mI2CChannel);
}

void initialize_soil_sensor_connection(SensorPod *sensorPod) {
    sensorPod->mSoilSensorActive = (init_soil_sensor(sensorPod->mInterface, sensorPod->mSoilSensorAddress) == I2C_RESPONSE_OK);
}

void initialize_scd30_connection(SensorPod *sensorPod) {
    char tmpSerial[SCD30_SERIAL_BYTE_SIZE];

    sensorPod->mSCD30SensorActive = (read_scd30_serial(sensorPod->mInterface, sensorPod->mSCD30Address, tmpSerial) == I2C_RESPONSE_OK);

    if(sensorPod->mSCD30SensorActive) {
        DEBUG_PRINT("        +- Serial: %s\n", tmpSerial);
    } else {
        DEBUG_PRINT("        +- Could not read serial number (%s)\n", tmpSerial);
    }

    if(sensorPod->mSCD30SensorActive) {
        I2CResponse readResponse = trigger_scd30_continuous_measurement(sensorPod->mInterface, sensorPod->mSCD30Address, 0);
        if(readResponse != I2C_RESPONSE_OK) {
            DEBUG_PRINT("        +- Continuous measurement trigger failed: %d\n", readResponse);
        }
    }
}

bool initialize_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return false;
    }

    if(select_sensor_pod(sensorPod) != I2C_RESPONSE_OK) {
        return false;
    }

    initialize_soil_sensor_connection(sensorPod);
    initialize_scd30_connection(sensorPod);

    if(!sensorPod->mSoilSensorActive || !sensorPod->mSCD30SensorActive) {
        DEBUG_PRINT("        +- Sensor pod init failed\n");
        return false;
    }

    sensorPod->mCurrentData.mSCD30SensorDataValid = false;
    sensorPod->mCurrentData.mSoilSensorDataValid = false;

    return true;
}

bool reset_sensor_pod(SensorPod *sensorPod) {
    static const uint16_t BOOT_DELAY_MS = 50;
    if(!sensorPod) {
        return false;
    }

    I2CResponse resetSoilSensorResponse = reset_soil_sensor(sensorPod->mInterface, sensorPod->mSoilSensorAddress);
    I2CResponse resetSCDResponse = do_scd30_soft_reset(sensorPod->mInterface, sensorPod->mSCD30Address);
    I2CResponse resetMultiplexerResponse = reset_i2c_multiplexer(sensorPod->mInterface);

    sleep_ms(BOOT_DELAY_MS);

    bool initResponse = initialize_sensor_pod(sensorPod);

    return (
        (resetSoilSensorResponse == I2C_RESPONSE_OK) &&
        (resetSCDResponse == I2C_RESPONSE_OK) &&
        (resetMultiplexerResponse == I2C_RESPONSE_OK) &&
        initResponse
    );
}


void update_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return;
    }

    bool gotSoilReading = false;
    bool gotSCDReading = false;

    DEBUG_PRINT("      +- Selecting pod channel: 0x%02X...", sensorPod->mI2CChannel);
    I2CResponse selectResponse = select_sensor_pod(sensorPod);
    DEBUG_PRINT("done {%d}\n", selectResponse);
    if(selectResponse != I2C_RESPONSE_OK) {
        return;
    }

    // Read soil sensor
    sensorPod->mCurrentData.mSoilSensorDataValid = false;
    if(sensorPod->mSoilSensorActive) {
        DEBUG_PRINT("      +- Soil sensor active, reading...");
        uint16_t capValue = get_soil_sensor_capacitive_value(sensorPod->mInterface, sensorPod->mSoilSensorAddress);
        if(capValue != STEMMA_SOIL_SENSOR_INVALID_READING) {
            sensorPod->mCurrentData.mSoilSensorData = capValue;
            sensorPod->mCurrentData.mSoilSensorDataValid = true;

            gotSoilReading = true;
            DEBUG_PRINT("done\n");
        } else {
            DEBUG_PRINT("INVALID\n");
        }
    } else {
        DEBUG_PRINT("      +- Soil sensor inactive, initializing...");
        initialize_soil_sensor_connection(sensorPod);
        DEBUG_PRINT("done\n");
    }

    // Read SCD30 (if we have a valid reading)
    if(sensorPod->mSCD30SensorActive) {
        DEBUG_PRINT("      +- SCD30 active\n");
        if(get_scd30_data_ready_status(sensorPod->mInterface, sensorPod->mSCD30Address)) {
            DEBUG_PRINT("        +- SCD30 data ready, reading...");
            SCD30SensorData tmpData = get_scd30_reading(sensorPod->mInterface, sensorPod->mSCD30Address);

            if(tmpData.mValidReading) {
                sensorPod->mCurrentData.mCO2Level = tmpData.mCO2Reading;
                sensorPod->mCurrentData.mTemperature = tmpData.mTemperatureReading;
                sensorPod->mCurrentData.mHumidity = tmpData.mHumidityReading;
                sensorPod->mCurrentData.mSCD30SensorDataValid = true;

                gotSCDReading = true;
                DEBUG_PRINT("done\n");
            } else {
                sensorPod->mCurrentData.mSCD30SensorDataValid = false;
                DEBUG_PRINT("INVALID\n");
            }
        } else {
            DEBUG_PRINT("        +- SCD30 data not ready\n");
        }
    } else {
        DEBUG_PRINT("      +- SCD30 inactive, initializing...\n");
        initialize_scd30_connection(sensorPod);
        DEBUG_PRINT("          done\n");
    }

    // It's common for there to not be both readings available, so as long as we have at least one, we are
    // good to reset the watchdog timer
    if(gotSoilReading || gotSCDReading) {
        // Reset pod timeout
        sensorPod->mPodResetTimeout = make_timeout_time_ms(SENSOR_POD_TIMEOUT_MS);
        DEBUG_PRINT("      +- Good data!\n");
    }

    if(absolute_time_diff_us(sensorPod->mPodResetTimeout, get_absolute_time()) > 0) {
        DEBUG_PRINT("      +- Pod timed out, resetting...");
        reset_sensor_pod(sensorPod);
        sensorPod->mPodResetTimeout = make_timeout_time_ms(SENSOR_POD_TIMEOUT_MS);
    }
}

bool is_sensor_pod_connected(SensorPod *sensorPod) {
    if(!sensorPod || !sensorPod->mInterface) {
        return false;
    }

    return is_i2c_channel_connected(sensorPod->mInterface, sensorPod->mI2CChannel);
}

bool sensor_pod_has_valid_data(SensorPod *sensorPod) {
    return (sensorPod->mCurrentData.mSoilSensorDataValid || sensorPod->mCurrentData.mSCD30SensorDataValid);
}
