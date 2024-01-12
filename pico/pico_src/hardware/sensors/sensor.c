#include "sensor.h"

#include <stdio.h>
#include <string.h>

#include "debug_io.h"

bool is_sensor_connected(Sensor *sensor, ConnectedHardwareMonitor *monitor);
bool initialize_sensor_hardware(Sensor *sensor);
void initialize_sensor_data(Sensor *sensor);
void debug_sensors(Sensor *sensors, uint8_t numSensors, ConnectedHardwareMonitor *monitor);
I2CInterface* get_sensor_i2c(Sensor *sensor);


bool is_sensor_connected(Sensor *sensor, ConnectedHardwareMonitor *monitor) {
    if(!sensor || !monitor) {
        return false;
    }

    bool connected = false;

    switch(sensor->mSensorDefinition.mHardwareConnectionID) {
        case ALWAYS_CONNECTED_CONNECT_ID:
            connected = true;
            break;
        case NEVER_CONNECTED_CONNECT_ID:
            connected = false;
            break;
        default:
            connected = is_hardware_connected(monitor, sensor->mSensorDefinition.mHardwareConnectionID);
            break;
    }

    return connected;
}

bool initialize_sensor_hardware(Sensor *sensor) {
    bool initialized = false;
    
    initialize_sensor_data(sensor);

    // Initialize underlying sensor hardware
    switch(sensor->mSensorDefinition.mSensorType) {
        case SONAR_SENSOR:
            initialize_sonar_sensor(&sensor->mSensorDefinition.mSensor.mSonarSensor);
            initialized = true;
            break;
        
        case SENSOR_POD:
            initialized = initialize_sensor_pod(&sensor->mSensorDefinition.mSensor.mSensorPod);
            break;

        case BATTERY_SENSOR:
            initialize_battery_sensor(&sensor->mSensorDefinition.mSensor.mBatterySensor);
            initialized = true;
            break;
    }

    return initialized;
}

void initialize_sensor_data(Sensor *sensor) {
    if(!sensor) {
        return;
    }

    sensor->mCurrentSensorData.mSensorStatus = SENSOR_DISCONNECTED;
}
void debug_sensors(Sensor *sensors, uint8_t numSensors, ConnectedHardwareMonitor *monitor) {
    DEBUG_PRINT("Sensor Values\n");
    DEBUG_PRINT("-------------\n");

    for(int i = 0; i < numSensors; ++i) {
        Sensor *sensor = &sensors[i];
        SensorData *sensorData = &sensor->mCurrentSensorData;

        DEBUG_PRINT("  * Sensor %d (type: %d)\n", i, sensor->mSensorDefinition.mSensorType);

        if(is_sensor_connected(sensor, monitor)) {
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
                case SENSOR_POD:
                    DEBUG_PRINT("SCD30 data: ")
                    if(sensor->mSensorDefinition.mSensor.mSensorPod.mCurrentData.mSCD30SensorDataValid) {
                        DEBUG_PRINT("\n");
                        DEBUG_PRINT("           CO2: %f ppm\n", sensor->mSensorDefinition.mSensor.mSensorPod.mCurrentData.mCO2Level);
                        DEBUG_PRINT("   Temperature: %f °C\n", sensor->mSensorDefinition.mSensor.mSensorPod.mCurrentData.mTemperature);
                        DEBUG_PRINT("            RH: %f \n", sensor->mSensorDefinition.mSensor.mSensorPod.mCurrentData.mHumidity);
                    } else {
                        DEBUG_PRINT("Invalid\n");
                    }
                    DEBUG_PRINT("Soil moisture data: ")
                    if(sensor->mSensorDefinition.mSensor.mSensorPod.mCurrentData.mSoilSensorDataValid) {
                        DEBUG_PRINT("%d\n", sensor->mSensorDefinition.mSensor.mSensorPod.mCurrentData.mSoilSensorData);
                    } else {
                        DEBUG_PRINT("Invalid\n");
                    }
                    break;
                case BATTERY_SENSOR:
                    DEBUG_PRINT("RTC battery voltage: %fv\n", sensor->mSensorDefinition.mSensor.mBatterySensor.mCurrentVoltage);
                    break;
            }
        } else {
            DEBUG_PRINT("    - DISCONNECTED\n");
        }
    }
    DEBUG_PRINT("\n**************************************\n\n");
}

I2CInterface* get_sensor_i2c(Sensor *sensor) {
    I2CInterface *i2cInterface = 0;

    if(sensor && (sensor->mSensorDefinition.mSensorType == SENSOR_POD)) {
        i2cInterface = sensor->mSensorDefinition.mSensor.mSensorPod.mInterface;
    }

    return i2cInterface;
}


void update_sensors(Sensor *sensors, uint8_t numSensors, bool debugOutput, ConnectedHardwareMonitor *monitor) {
    float val;

    DEBUG_PRINT("Sensor update:\n");

    for(int i = 0; i < numSensors; ++i) {
        Sensor *sensor = &sensors[i];

        // First we should check to see if the I2C Interface for this sensor has gotten stuck
        check_interface_watchdog(get_sensor_i2c(sensor));

        // After a (potential) reset we should be good to try the sensor
        SensorData *sensorData = &sensor->mCurrentSensorData;
        DEBUG_PRINT("  +- Updating sensor %d (type: %d)\n", sensor->mSensorDefinition.mSensorID, sensor->mSensorDefinition.mSensorType);
        memset(sensorData, 0, sizeof(SensorData));

        // Check connection
        if(is_sensor_connected(sensor, monitor)) {
            DEBUG_PRINT("    +- Connected\n");

            // If the sensor has just been connected, initialize its hardware
            if(!sensor->mHardwareInitialized) {
                sensor->mHardwareInitialized = initialize_sensor_hardware(sensor);
                DEBUG_PRINT("    +- Initializing: %s\n", sensor->mHardwareInitialized ? "SUCCESS" : "FAILED");
            }

            // If the sensor is still in an invalid hardware state, it's probably non-functional.
            // There is no point in proceeding here.
            if(!sensor->mHardwareInitialized) {
                sensorData->mSensorStatus = SENSOR_CONNECTED_MALFUNCTIONING;
                continue;
            }

            switch(sensor->mSensorDefinition.mSensorType) {
                case SONAR_SENSOR:
                    DEBUG_PRINT("    +- Sonar sensor update running...\n");
                    update_sonar_sensor(&sensor->mSensorDefinition.mSensor.mSonarSensor);
                    if(sensor->mSensorDefinition.mSensor.mSonarSensor.mState == VALID_SONAR_DATA) {
                        sensorData->mSensorReading.mSonarSensorDistance = sensor->mSensorDefinition.mSensor.mSonarSensor.mCurrentDistance;
                        sensorData->mSensorStatus = SENSOR_CONNECTED_VALID_DATA;
                    } else {
                        sensorData->mSensorStatus = SENSOR_CONNECTED_MALFUNCTIONING;
                    }
                    break;

                case SENSOR_POD:
                    DEBUG_PRINT("    +- Sensor pod update running...\n");
                    update_sensor_pod(&sensor->mSensorDefinition.mSensor.mSensorPod);
                    if(sensor_pod_has_valid_data(&sensor->mSensorDefinition.mSensor.mSensorPod)) {
                        sensorData->mSensorReading.mSensorPodData = sensor->mSensorDefinition.mSensor.mSensorPod.mCurrentData;
                        sensorData->mSensorStatus = SENSOR_CONNECTED_VALID_DATA;
                    } else {
                        sensorData->mSensorStatus = SENSOR_CONNECTED_MALFUNCTIONING;
                    }
                    break;

                case BATTERY_SENSOR:
                    DEBUG_PRINT("    +- Battery sensor update running...\n");
                    update_battery_level(&sensor->mSensorDefinition.mSensor.mBatterySensor);
                    sensorData->mSensorStatus = SENSOR_CONNECTED_VALID_DATA;
                    sensorData->mSensorReading.mBatteryVoltage = sensor->mSensorDefinition.mSensor.mBatterySensor.mCurrentVoltage;
            }
        } else {
            // Sensor is disconnected, make sure all flags are in the invalid state
            DEBUG_PRINT("    +- Not connected\n");
            sensor->mHardwareInitialized = false;
            sensorData->mSensorStatus = SENSOR_DISCONNECTED;
        }
    }
    DEBUG_PRINT("--------------------------------\n\n");

    if(debugOutput) {
        debug_sensors(sensors, numSensors, monitor);
    }
}
