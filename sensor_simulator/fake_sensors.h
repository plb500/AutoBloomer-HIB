#ifndef __FAKE_SENSORS_H
#define __FAKE_SENSORS_H

#include "sensor_msgpack_data.h"


                        /////////////////////////////////////////////////
                        // Fake sensor descriptions for debug hardware //
                        /////////////////////////////////////////////////

// Static reading definitions
SensorReadingDescription TEMPERATURE_READING_DESCRIPTION = {
    "Temperature (C)",                      // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=10.0f},                  // mMinValue
    {.mFloatValue=65.0f}                    // mMaxValue
};

SensorReadingDescription HUMIDITY_READING_DESCRIPTION = {
    "RH (%)",                               // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=100.0f}                   // mMaxValue
};

SensorReadingDescription SOIL_MOISTURE_READING_DESCRIPTION = {
    "Moisture level",                       // mReadingName,
    INT_READING,                            // mType   
    {.mIntValue=0},                         // mMinValue
    {.mIntValue=500}                      // mMaxValue
};

SensorReadingDescription WEIGHT_READING_DESCRIPTION = {
    "Weight (kg)",                          // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=20.0f}                   // mMaxValue
};

SensorReadingDescription BOOL_READING_DESCRIPTION = {
    "Active status",                        // mReadingName
    BOOL_READING,                           // mType   
    {.mBoolValue=false},                    // mMinValue
    {.mBoolValue=true}                      // mMaxValue
};


// Reading objects
SensorReading MOISTURE_SENSOR_L_READINGS[] = {
    {
        &SOIL_MOISTURE_READING_DESCRIPTION,
        {.mIntValue=129}
    }    
};

SensorReading MOISTURE_SENSOR_R_READINGS[] = {
    {
        &SOIL_MOISTURE_READING_DESCRIPTION,
        {.mIntValue=354}
    }    
};

SensorReading DHT22_SENSOR_L_READINGS[] = {
    {
        &TEMPERATURE_READING_DESCRIPTION,
        {.mFloatValue=24.5}
    },
    {
        &HUMIDITY_READING_DESCRIPTION,
        {.mFloatValue=64.5}
    }
};

SensorReading DHT22_SENSOR_R_READINGS[] = {
    {
        &TEMPERATURE_READING_DESCRIPTION,
        {.mFloatValue=25.8}
    },
    {
        &HUMIDITY_READING_DESCRIPTION,
        {.mFloatValue=76.3}
    }
};

SensorReading WEIGHT_SENSOR_L_READINGS[] = {
    {
        &WEIGHT_READING_DESCRIPTION,
        {.mFloatValue=18.24}
    }
};

SensorReading WEIGHT_SENSOR_R_READINGS[] = {
    {
        &WEIGHT_READING_DESCRIPTION,
        {.mFloatValue=11.87}
    }
};

SensorReading BINARY_SENSOR_L_READINGS[] = {
    {
        &BOOL_READING_DESCRIPTION,
        {.mBoolValue=true}
    }
};

SensorReading BINARY_SENSOR_R_READINGS[] = {
    {
        &BOOL_READING_DESCRIPTION,
        {.mBoolValue=false}
    }
};


                        //////////////////////////////////////////
                        // Fake sensor data for debug hardware //
                        /////////////////////////////////////////

SensorData MOISTURE_SENSOR_L = {
    0,                                      // mSensorID
    "Left moisture sensor",                 // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    MOISTURE_SENSOR_L_READINGS              // Each individual sensor reading
};

SensorData MOISTURE_SENSOR_R = {
    1,                                      // mSensorID
    "Right moisture sensor",                // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    MOISTURE_SENSOR_R_READINGS              // Each individual sensor reading
};

SensorData DHT22_SENSOR_L = {
    2,                                      // mSensorID
    "Left DHT22 Temp/Humidity sensor",      // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    2,                                      // mNumReadings
    DHT22_SENSOR_L_READINGS                 // mReadingDetails
};

SensorData DHT22_SENSOR_R = {
    3,                                      // mSensorID
    "Right DHT22 Temp/Humidity sensor",     // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    2,                                      // mNumReadings
    DHT22_SENSOR_R_READINGS                 // mReadingDetails
};

SensorData WEIGHT_SENSOR_L = {
    4,                                      // mSensorID
    "Left weight sensor",                   // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    WEIGHT_SENSOR_L_READINGS                // mReadingDetails
};

SensorData WEIGHT_SENSOR_R = {
    5,                                      // mSensorID
    "Right weight sensor",                  // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    WEIGHT_SENSOR_R_READINGS                // mReadingDetails
};

SensorData BINARY_SENSOR_L = {
    6,                                      // mSensorID
    "Left On/Off sensor",                   // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    BINARY_SENSOR_L_READINGS                // mReadingDetails
};

SensorData BINARY_SENSOR_R = {
    7,                                      // mSensorID
    "Right On/Off sensor",                  // mSensorName
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    BINARY_SENSOR_R_READINGS                // mReadingDetails
};




// Container for the fake sensors/descriptions
const int NUM_FAKE_SENSORS = 8;

SensorData * FAKE_SENSORS[] = {
    &MOISTURE_SENSOR_L,
    &MOISTURE_SENSOR_R,
    &DHT22_SENSOR_L,
    &DHT22_SENSOR_R,
    &WEIGHT_SENSOR_L,
    &WEIGHT_SENSOR_R,
    &BINARY_SENSOR_L,
    &BINARY_SENSOR_R
};


#endif  // __FAKE_SENSORS_H
