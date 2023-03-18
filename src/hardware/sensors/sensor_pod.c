#include "sensor_pod.h"

bool select_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return false;
    }

    return select_i2c_channel(sensorPod->mInterface, sensorPod->mI2CChannel);
}

bool initialize_sensor_pod(SensorPod *sensorPod) {
    if(!sensorPod) {
        return false;
    }

    select_sensor_pod(sensorPod);

    char tmpSerial[SCD30_SERIAL_BYTE_SIZE];

    sensorPod->mSoilSensorActive = init_soil_sensor(sensorPod);
    sensorPod->mSCD30SensorActive = read_scd30_serial(sensorPod, tmpSerial);

    if(sensorPod->mSCD30SensorActive) {
        trigger_scd30_continuous_measurement(sensorPod, 0);
    }

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
        uint16_t capValue = get_soil_sensor_capacitive_value(sensorPod);
        if(capValue != STEMMA_SOIL_SENSOR_INVALID_READING) {
            sensorPod->mCurrentData.mSoilSensorData = capValue;
            sensorPod->mCurrentData.mSoilSensorDataValid = true;
        }
    }

    // Read SCD30 (if we have a valid reading)
    if(sensorPod->mSCD30SensorActive && get_scd30_data_ready_status(sensorPod)) {
        sensorPod->mCurrentData.mSCD30SensorData = get_scd30_reading(sensorPod);
        sensorPod->mCurrentData.mSCD30SensorDataValid = true;
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
