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
    return !gpio_get(sensor->mJackDetectPin);
}

bool initialize_sensor_hardware(Sensor *sensor) {
    bool initialized = false;
    
    // Initialize underlying sensor hardware
    if(is_sensor_connected(sensor)) {
        switch(sensor->mSensorType) {
            case SONAR_SENSOR:
                initialize_sonar_sensor(&sensor->mSensor.mSonarSensor);
                initialized = true;
                break;
                
            case TEMP_HUMIDITY_SENSOR:
                init_DHT22(&sensor->mSensor.mDHTSensor);
                initialized = true;
                break;

            case MOISTURE_SENSOR:
                initialized = soil_sensor_begin(&sensor->mSensor.mMoistureSensor);
                if(!initialized) {
                    DEBUG_PRINT("ERROR INITIALIZING MOISURE SENSOR (%d)!\n", sensor->mSensorID);
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
        initialize_sensor_jack_detect_pin(sensors[i]->mJackDetectPin);
    }
}

void debug_sensors(Sensor **sensors, SensorData *dataStorage, uint8_t numSensors) {
    DEBUG_PRINT("Sensor Updates\n");
    DEBUG_PRINT("--------------\n");

    for(int i = 0; i < numSensors; ++i) {
        Sensor *sensor = sensors[i];
        SensorData *sensorData = &dataStorage[i];

        DEBUG_PRINT("  * Sensor %d (type: %d)\n", i, sensor->mSensorType);

        if(is_sensor_connected(sensor)) {
            DEBUG_PRINT("    - CONNECTED -- ");

            switch(sensor->mSensorType) {
                case SONAR_SENSOR:
                    switch(sensor->mSensor.mSonarSensor.mState) {
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

void update_sensor_readings(Sensor **sensors, SensorData *dataStorage, uint8_t numSensors) {
    memset(dataStorage, 0, sizeof(SensorData) * numSensors);
    float val;

    for(int i = 0; i < numSensors; ++i) {
        Sensor *sensor = sensors[i];
        SensorData *sensorData = &dataStorage[i];
        sensorData->mSensorID = sensor->mSensorID;
        sensorData->mType = sensor->mSensorType;


        // Check connection
        if(is_sensor_connected(sensor)) {
            sensorData->mIsSensorConnected = true;

            // If the sensor has just been connected, initialize its hardware
            if(!sensor->mHardwareInitialized) {
                sensor->mHardwareInitialized = initialize_sensor_hardware(sensor);
            }

            // If the sensor is still in an invalid hardware state, it's probably non-functional.
            // There is no point in proceeding here.
            if(!sensor->mHardwareInitialized) {
                sensorData->mIsSensorReadingValid = false;
                continue;
            }

            switch(sensor->mSensorType) {
                case SONAR_SENSOR:
                    update_sonar_sensor(&sensor->mSensor.mSonarSensor);
                    if(sensor->mSensor.mSonarSensor.mState == VALID_SONAR_DATA) {
                        sensorData->mSensorReading.mSonarSensorDistance = sensor->mSensor.mSonarSensor.mCurrentDistance;
                        sensorData->mIsSensorReadingValid = true;
                    } else {
                        sensorData->mIsSensorReadingValid = false;
                    }
                    break;

                case TEMP_HUMIDITY_SENSOR:
                    read_DHT22(&sensor->mSensor.mDHTSensor, &sensorData->mSensorReading.mTempHumidityData);
                    if(sensorData->mSensorReading.mTempHumidityData.mReadingError == NO_ERROR) {
                        sensorData->mIsSensorReadingValid = true;
                    } else {
                        sensorData->mIsSensorReadingValid = false;
                    }
                    break;

                case MOISTURE_SENSOR:
                    sensorData->mSensorReading.mMoistureSensorValue = get_soil_sensor_capacitive_value(&sensor->mSensor.mMoistureSensor);
                    if(sensorData->mSensorReading.mMoistureSensorValue != -1) {
                        sensorData->mIsSensorReadingValid = true;
                    } else {
                        sensorData->mIsSensorReadingValid = false;
                    }
                    break;
            }
        } else {
            // Sensor is disconnected, make sure all flags are in the invalid state
            sensor->mHardwareInitialized = false;
            sensorData->mIsSensorReadingValid = false;
            sensorData->mIsSensorConnected = false;
        }
    }

    debug_sensors(sensors, dataStorage, numSensors);
}
