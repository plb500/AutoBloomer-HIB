#include "sensor.h"

#include <stdio.h>
#include <string.h>

#include "debug_io.h"

void initialize_sensor_jack_detect_pin(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

bool is_sensor_connected(Sensor *sensor) {
    return !gpio_get(sensor->mSensorDefinition.mJackDetectPin);
}

bool initialize_sensor_hardware(Sensor *sensor) {
    bool initialized = false;
    
    // Initialize underlying sensor hardware
    if(is_sensor_connected(sensor)) {
        switch(sensor->mSensorDefinition.mSensorType) {
            case SONAR_SENSOR:
                initialize_sonar_sensor(&sensor->mSensorDefinition.mSensor.mSonarSensor);
                initialized = true;
                break;
                
            case TEMP_HUMIDITY_SENSOR:
                init_DHT22(&sensor->mSensorDefinition.mSensor.mDHTSensor);
                initialized = true;
                break;

            case MOISTURE_SENSOR:
                initialized = soil_sensor_begin(&sensor->mSensorDefinition.mSensor.mMoistureSensor);
                if(!initialized) {
                    DEBUG_PRINT("ERROR INITIALIZING MOISURE SENSOR (%d)!\n", sensor->mSensorDefinition.mSensorID);
                }
                break;
        }
    }

    return initialized;
}

void initialize_sensors(Sensor **sensors, uint8_t numSensors, i2c_inst_t *i2c, const int baud, const int sdaPin, const int sclPin) {
    // Initialize the I2C bus
    init_sensor_bus(i2c, baud, sdaPin, sclPin);

    // Setup sensors
    for(int i = 0; i < numSensors; ++i) {
        // Initialize jack detect pin
        initialize_sensor_jack_detect_pin(sensors[i]->mSensorDefinition.mJackDetectPin);
    }
}

void debug_sensors(Sensor **sensors, uint8_t numSensors) {
    DEBUG_PRINT("Sensor Updates\n");
    DEBUG_PRINT("--------------\n");

    for(int i = 0; i < numSensors; ++i) {
        Sensor *sensor = sensors[i];
        SensorData *sensorData = &sensor->mCurrentSensorData;

        DEBUG_PRINT("  * Sensor %d (type: %d)\n", i, sensor->mSensorDefinition.mSensorType);

        if(is_sensor_connected(sensor)) {
            DEBUG_PRINT("    - CONNECTED -- ");

            switch(sensor->mSensorDefinition.mSensorType) {
                case SONAR_SENSOR:
                    switch(sensor->mSensorDefinition.mSensor.mSonarSensor.mState) {
                        case AWAITING_SONAR_DATA:
                            DEBUG_PRINT("Awaiting sonar data\n");
                            break;
                        case VALID_SONAR_DATA:
                            DEBUG_PRINT("%dmm\n", sensorData->mSensorReading.mSonarSensorDistance);
                            break;
                        case INVALID_SONAR_CHECKSUM:
                            DEBUG_PRINT("Invalid checksum\n");
                            break;
                    }
                    break;
                case TEMP_HUMIDITY_SENSOR:
                    if(sensorData->mSensorReading.mTempHumidityData.mReadingError == NO_ERROR) {
                        DEBUG_PRINT("temp %fC, rH %f%%\n", 
                            sensorData->mSensorReading.mTempHumidityData.mTemperatureC,
                            sensorData->mSensorReading.mTempHumidityData.mRelativeHumidity);
                    } else {
                        DEBUG_PRINT("r3Ad1nG 3rr0r\n");
                    }
                    break;
                case MOISTURE_SENSOR:
                    if(sensorData->mSensorReading.mMoistureSensorValue != -1) {
                        DEBUG_PRINT("%d\n", sensorData->mSensorReading.mMoistureSensorValue);
                    } else {
                        DEBUG_PRINT("r3Ad1nG 3rr0r\n");
                    }
                    break;
            }
        } else {
            DEBUG_PRINT("    - DISCONNECTED\n");
        }
    }
    DEBUG_PRINT("\n**************************************\n\n");
}

void update_sensor_readings(Sensor **sensors, uint8_t numSensors) {
    float val;

    for(int i = 0; i < numSensors; ++i) {
        Sensor *sensor = sensors[i];
        SensorData *sensorData = &sensor->mCurrentSensorData;
        memset(sensorData, 0, sizeof(SensorData));
        // sensorData->mSensorID = sensor->mSensorID;
        // sensorData->mType = sensor->mSensorType;


        // Check connection
        if(is_sensor_connected(sensor)) {
            // sensorData->mIsSensorConnected = true;

            // If the sensor has just been connected, initialize its hardware
            if(!sensor->mHardwareInitialized) {
                sensor->mHardwareInitialized = initialize_sensor_hardware(sensor);
            }

            // If the sensor is still in an invalid hardware state, it's probably non-functional.
            // There is no point in proceeding here.
            if(!sensor->mHardwareInitialized) {
                sensorData->mSensorStatus = SENSOR_CONNECTED_MALFUNCTIONING;
                continue;
            }

            switch(sensor->mSensorDefinition.mSensorType) {
                case SONAR_SENSOR:
                    update_sonar_sensor(&sensor->mSensorDefinition.mSensor.mSonarSensor);
                    if(sensor->mSensorDefinition.mSensor.mSonarSensor.mState == VALID_SONAR_DATA) {
                        sensorData->mSensorReading.mSonarSensorDistance = sensor->mSensorDefinition.mSensor.mSonarSensor.mCurrentDistance;
                        sensorData->mSensorStatus = SENSOR_CONNECTED_VALID_DATA;
                    } else {
                        sensorData->mSensorStatus = SENSOR_CONNECTED_MALFUNCTIONING;
                    }
                    break;

                case TEMP_HUMIDITY_SENSOR:
                    read_DHT22(&sensor->mSensorDefinition.mSensor.mDHTSensor, &sensorData->mSensorReading.mTempHumidityData);
                    if(sensorData->mSensorReading.mTempHumidityData.mReadingError == NO_ERROR) {
                        sensorData->mSensorStatus = SENSOR_CONNECTED_VALID_DATA;
                    } else {
                        sensorData->mSensorStatus = SENSOR_CONNECTED_MALFUNCTIONING;
                    }
                    break;

                case MOISTURE_SENSOR:
                    sensorData->mSensorReading.mMoistureSensorValue = get_soil_sensor_capacitive_value(&sensor->mSensorDefinition.mSensor.mMoistureSensor);
                    if(sensorData->mSensorReading.mMoistureSensorValue != -1) {
                        sensorData->mSensorStatus = SENSOR_CONNECTED_VALID_DATA;
                    } else {
                        sensorData->mSensorStatus = SENSOR_CONNECTED_MALFUNCTIONING;
                    }
                    break;
            }
        } else {
            // Sensor is disconnected, make sure all flags are in the invalid state
            sensor->mHardwareInitialized = false;
            sensorData->mSensorStatus = SENSOR_DISCONNECTED;
        }
    }

    debug_sensors(sensors, numSensors);
}
