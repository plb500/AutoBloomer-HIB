#ifndef __FAKE_SENSORS_H
#define __FAKE_SENSORS_H

#include "sensor_msgpack_data.h"

                        ////////////////////////////////////////////////
                        // Fake sensor definitions for debug hardware //
                        ////////////////////////////////////////////////

// Static reading definitions
ReadingDetails TEMPERATURE_READING_DETAILS = {
    FLOAT_READING,                      // mType   
    {.mFloatValue=-40.0f},              // mMinValue
    {.mFloatValue=80.0f}                // mMaxValue
};

ReadingDetails HUMIDITY_READING_DETAILS = {
    FLOAT_READING,                      // mType   
    {.mFloatValue=0.0f},                // mMinValue
    {.mFloatValue=100.0f}               // mMaxValue
};

ReadingDetails SOIL_MOISTURE_READING_DETAILS = {
    INT_READING,                        // mType   
    {.mIntValue=0},                     // mMinValue
    {.mIntValue=65535}                  // mMaxValue
};

ReadingDetails WEIGHT_READING_DETAILS = {
    FLOAT_READING,                      // mType   
    {.mFloatValue=0.0f},                // mMinValue
    {.mFloatValue=200.0f}               // mMaxValue
};

ReadingDetails BOOL_READING_DETAILS = {
    BOOL_READING,                       // mType   
    {.mBoolValue=false},                // mMinValue
    {.mBoolValue=true}                  // mMaxValue
};



// Fake sensors
SensorData MOISTURE_SENSOR_L = {
    0,                                              // mSensorID
    "Left moisture sensor",                         // mSensorDescription
    0,                                              // mSensorStatus
    1,                                              // mNumReadings
    {                                               // mSensorReadings
        "Moisture value",                           // mSensorName,
        {.mIntValue = 129},                         // mValue
        SOIL_MOISTURE_READING_DETAILS               // mDetails
    }
};

SensorData MOISTURE_SENSOR_R = {
    1,                                              // mSensorID
    "Right moisture sensor",                        // mSensorDescription
    0,                                              // mSensorStatus
    1,                                              // mNumReadings
    {                                               // mSensorReadings
        "Moisture value",                           // mSensorName,
        {.mIntValue = 326},                         // mValue
        SOIL_MOISTURE_READING_DETAILS               // mDetails
    }
};

SensorData DHT22_SENSOR_L = {
    2,                                              // mSensorID
    "Left DHT22 Temp/Humidity sensor",              // mSesnorDescription
    0,                                              // mSensorStatus
    2,                                              // mNumReadings
    {                                               // mSensorReadings
        {
            "Temperature value",                    // mSensorName
            {.mFloatValue = 20.0f},                 // mValue
            TEMPERATURE_READING_DETAILS             // mDetails
        },
        {
            "RH value",                             // mSensorName
            {.mFloatValue = 60.0f},                 // mValue
            HUMIDITY_READING_DETAILS                // mDetails
        }
    }
};

SensorData DHT22_SENSOR_R = {
    3,                                              // mSensorID
    "Right DHT22 Temp/Humidity sensor",             // mSensorDescription
    0,                                              // mSensorStatus
    2,                                              // mNumReadings
    {                                               // mSensorReadings
        {
            "Temperature value",                    // mSensorName
            {.mFloatValue = 22.3f},                 // mValue
            TEMPERATURE_READING_DETAILS             // mDetails
        },
        {
            "RH value",                             // mSensorName
            {.mFloatValue = 65.0f},                 // mValue
            HUMIDITY_READING_DETAILS                // mDetails
        }
    }
};

SensorData WEIGHT_SENSOR_L = {
    4,                                              // mSensorID
    "Left weight sensor",                           // mSensorDescription
    0,                                              // mSensorStatus
    1,                                              // mNumReadings
    {                                               // mSensorReadings
        {
            "Weight value (kg)",                    // mSensorName
            {.mFloatValue = 5.1f},                  // mValue
            WEIGHT_READING_DETAILS                  // mDetails
        }
    }
};


SensorData WEIGHT_SENSOR_R = {
    5,                                              // mSensorID
    "Right weight sensor",                          // mSensorDescription
    0,                                              // mSensorStatus
    1,                                              // mNumReadings
    {                                               // mSensorReadings
        {
            "Weight value (kg)",                    // mSensorName
            {.mFloatValue = 7.8f},                  // mValue
            WEIGHT_READING_DETAILS                  // mDetails
        }
    }
};

SensorData SOME_BINARY_SENSOR_L = {
    6,                                              // mSensorID
    "Left On/Off sensor",                           // mSensorDescription
    0,                                              // mSensorStatus
    1,                                              // mNumReadings
    {                                               // mSensorReadings
        {
            "Active status",                        // mSensorName
            {.mBoolValue = true},                   // mValue
            BOOL_READING_DETAILS                    // mDetails
        }
    }
};

SensorData SOME_BINARY_SENSOR_R = {
    7,                                              // mSensorID
    "Right On/Off sensor",                           // mSensorDescription
    0,                                              // mSensorStatus
    1,                                              // mNumReadings
    {                                               // mSensorReadings
        {
            "Active status",                        // mSensorName
            {.mBoolValue = false},                  // mValue
            BOOL_READING_DETAILS                    // mDetails
        }
    }
};



// Container for the fake sensors
const int NUM_FAKE_SENSORS = 8;
SensorData * FAKE_SENSORS[] = {
    &MOISTURE_SENSOR_L,
    &MOISTURE_SENSOR_R,
    &DHT22_SENSOR_L,
    &DHT22_SENSOR_R,
    &WEIGHT_SENSOR_L,
    &WEIGHT_SENSOR_R,
    &SOME_BINARY_SENSOR_L,
    &SOME_BINARY_SENSOR_R
};

#endif  // __FAKE_SENSORS_H
