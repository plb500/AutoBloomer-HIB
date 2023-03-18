#include "sensor_pod.h"

#include "stemma_soil_sensor.h"
#include "scd30_sensor.h"

bool select_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return false;
    }

    return select_i2c_channel(sensorPod->mInterface, sensorPod->mI2CChannel);
}

void initialize_soil_sensor_connection(SensorPod *sensorPod) {
    sensorPod->mSoilSensorActive = init_soil_sensor(sensorPod->mInterface, sensorPod->mSoilSensorAddress);
}

void initialize_scd30_connection(SensorPod *sensorPod) {
    char tmpSerial[SCD30_SERIAL_BYTE_SIZE];

    sensorPod->mSCD30SensorActive = read_scd30_serial(sensorPod->mInterface, sensorPod->mSCD30Address, tmpSerial);

    if(sensorPod->mSCD30SensorActive) {
        trigger_scd30_continuous_measurement(sensorPod->mInterface, sensorPod->mSCD30Address, 0);
    }
}


bool initialize_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return false;
    }

    select_sensor_pod(sensorPod);

    initialize_soil_sensor_connection(sensorPod);
    initialize_scd30_connection(sensorPod);

    sensorPod->mCurrentData.mSCD30SensorDataValid = false;
    sensorPod->mCurrentData.mSoilSensorDataValid = false;
}

void update_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return;
    }

    select_sensor_pod(sensorPod);

    // Read soil sensor
    sensorPod->mCurrentData.mSoilSensorDataValid = false;
    if(sensorPod->mSoilSensorActive) {
        uint16_t capValue = get_soil_sensor_capacitive_value(sensorPod->mInterface, sensorPod->mSoilSensorAddress);
        if(capValue != STEMMA_SOIL_SENSOR_INVALID_READING) {
            sensorPod->mCurrentData.mSoilSensorData = capValue;
            sensorPod->mCurrentData.mSoilSensorDataValid = true;
        }
    } else {
        initialize_soil_sensor_connection(sensorPod);
    }

    // Read SCD30 (if we have a valid reading)
    if(sensorPod->mSCD30SensorActive && get_scd30_data_ready_status(sensorPod->mInterface, sensorPod->mSCD30Address)) {
        SCD30SensorData tmpData = get_scd30_reading(sensorPod->mInterface, sensorPod->mSCD30Address);

        if(tmpData.mValidReading) {
            sensorPod->mCurrentData.mCO2Level = tmpData.mCO2Reading;
            sensorPod->mCurrentData.mTemperature = tmpData.mTemperatureReading;
            sensorPod->mCurrentData.mHumidity = tmpData.mHumidityReading;
            sensorPod->mCurrentData.mSCD30SensorDataValid = true;
        } else {
            sensorPod->mCurrentData.mSCD30SensorDataValid = false;
        }
    } else {
        initialize_scd30_connection(sensorPod);
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
