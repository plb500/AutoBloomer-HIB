#ifndef __FAKE_SENSORS_H
#define __FAKE_SENSORS_H

#include "sensor_msgpack_data.h"


                        /////////////////////////////////////////////////
                        // Fake sensor descriptions for debug hardware //
                        /////////////////////////////////////////////////

// Static reading definitions
ReadingDetails TEMPERATURE_READING_DETAILS = {
    "Temperature value",                    // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=-40.0f},                  // mMinValue
    {.mFloatValue=80.0f}                    // mMaxValue
};

ReadingDetails HUMIDITY_READING_DETAILS = {
    "RH value",                             // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=100.0f}                   // mMaxValue
};

ReadingDetails SOIL_MOISTURE_READING_DETAILS = {
    "Moisture value",                       // mReadingName,
    INT_READING,                            // mType   
    {.mIntValue=0},                         // mMinValue
    {.mIntValue=65535}                      // mMaxValue
};

ReadingDetails WEIGHT_READING_DETAILS = {
    "Weight value (kg)",                    // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=200.0f}                   // mMaxValue
};

ReadingDetails BOOL_READING_DETAILS = {
    "Active status",                        // mReadingName
    BOOL_READING,                           // mType   
    {.mBoolValue=false},                    // mMinValue
    {.mBoolValue=true}                      // mMaxValue
};


// Fake sensor reading details
ReadingDetails * MOISTURE_SENSOR_READING_DETAILS[] = {
        &SOIL_MOISTURE_READING_DETAILS
};

ReadingDetails * DHT22_SENSOR_READING_DETAILS[] = {
        &TEMPERATURE_READING_DETAILS,
        &HUMIDITY_READING_DETAILS
};

ReadingDetails * WEIGHT_SENSOR_READING_DETAILS[] = {
        &WEIGHT_READING_DETAILS
};

ReadingDetails * SOME_BINARY_SENSOR_READING_DETAILS[] = {
        &BOOL_READING_DETAILS
};


// Fake sensor description structs
SensorDescription MOISTURE_SENSOR_L_DESCRIPTION = {
    0,                                      // mSensorID
    "Left moisture sensor",                 // mSensorName
    1,                                      // mNumReadings
    MOISTURE_SENSOR_READING_DETAILS         // mReadingDetails
};

SensorDescription MOISTURE_SENSOR_R_DESCRIPTION = {
    1,                                      // mSensorID
    "Right moisture sensor",                // mSensorName
    1,                                      // mNumReadings
    MOISTURE_SENSOR_READING_DETAILS         // mReadingDetails
};

SensorDescription DHT22_SENSOR_L_DESCRIPTION = {
    2,                                      // mSensorID
    "Left DHT22 Temp/Humidity sensor",      // mSensorName
    2,                                      // mNumReadings
    DHT22_SENSOR_READING_DETAILS            // mReadingDetails
};

SensorDescription DHT22_SENSOR_R_DESCRIPTION = {
    3,                                      // mSensorID
    "Right DHT22 Temp/Humidity sensor",     // mSensorName
    2,                                      // mNumReadings
    DHT22_SENSOR_READING_DETAILS            // mReadingDetails
};

SensorDescription WEIGHT_SENSOR_L_DESCRIPTION = {
    4,                                      // mSensorID
    "Left weight sensor",                   // mSensorName
    1,                                      // mNumReadings
    WEIGHT_SENSOR_READING_DETAILS           // mReadingDetails
};

SensorDescription WEIGHT_SENSOR_R_DESCRIPTION = {
    5,                                      // mSensorID
    "Right weight sensor",                  // mSensorName
    1,                                      // mNumReadings
    WEIGHT_SENSOR_READING_DETAILS           // mReadingDetails
};

SensorDescription SOME_BINARY_SENSOR_L_DESCRIPTION = {
    6,                                      // mSensorID
    "Left On/Off sensor",                   // mSensorName
    1,                                      // mNumReadings
    SOME_BINARY_SENSOR_READING_DETAILS      // mReadingDetails
};

SensorDescription SOME_BINARY_SENSOR_R_DESCRIPTION = {
    7,                                      // mSensorID
    "Right On/Off sensor",                  // mSensorName
    1,                                      // mNumReadings
    SOME_BINARY_SENSOR_READING_DETAILS      // mReadingDetails
};



                        //////////////////////////////////////////
                        // Fake sensor data for debug hardware //
                        /////////////////////////////////////////


SensorReading MOISTURE_SENSOR_L_SENSOR_READINGS[] = {
    {
        INT_READING,                        // mType
        {.mIntValue = 129},                 // mValue
    }
};

SensorReading MOISTURE_SENSOR_R_SENSOR_READINGS[] = {
    {
        INT_READING,                        // mType
        {.mIntValue = 326},                 // mValue
    }
};

SensorReading DHT22_SENSOR_L_SENSOR_READINGS[] = {
    {
        FLOAT_READING,                      // mType
        {.mFloatValue = 20.0f},             // mValue
    },
    {
        FLOAT_READING,                      // mType
        {.mFloatValue = 60.0f},             // mValue
    }
};

SensorReading DHT22_SENSOR_R_SENSOR_READINGS[] = {
    {
        FLOAT_READING,                      // mType
        {.mFloatValue = 22.3f},             // mValue
    },
    {
        FLOAT_READING,                      // mType
        {.mFloatValue = 65.1f},             // mValue
    }
};

SensorReading WEIGHT_SENSOR_L_SENSOR_READINGS[] = {
    {
        FLOAT_READING,                      // mType
        {.mFloatValue = 5.2f},              // mValue
    },
};

SensorReading WEIGHT_SENSOR_R_SENSOR_READINGS[] = {
    {
        FLOAT_READING,                      // mType
        {.mFloatValue = 8.12f},             // mValue
    },
};

SensorReading SOME_BINARY_SENSOR_L_SENSOR_READINGS[] = {
    {
        BOOL_READING,                       // mType
        {.mBoolValue = false},              // mValue
    }
};

SensorReading SOME_BINARY_SENSOR_R_SENSOR_READINGS[] = {
    {
        BOOL_READING,                       // mType
        {.mBoolValue = true},               // mValue
    }
};




// Fake sensors
SensorData MOISTURE_SENSOR_L = {
    0,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    MOISTURE_SENSOR_L_SENSOR_READINGS       // mSensorReadings
};

SensorData MOISTURE_SENSOR_R = {
    1,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    MOISTURE_SENSOR_R_SENSOR_READINGS       // mSensorReadings
};

SensorData DHT22_SENSOR_L = {
    2,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    2,                                      // mNumReadings
    DHT22_SENSOR_L_SENSOR_READINGS          // mSensorReadings
};

SensorData DHT22_SENSOR_R = {
    3,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    2,                                      // mNumReadings
    DHT22_SENSOR_R_SENSOR_READINGS          // mSensorReadings
};

SensorData WEIGHT_SENSOR_L = {
    4,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    WEIGHT_SENSOR_L_SENSOR_READINGS         // mSensorReadings
};


SensorData WEIGHT_SENSOR_R = {
    5,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    WEIGHT_SENSOR_R_SENSOR_READINGS         // mSensorReadings
};

SensorData SOME_BINARY_SENSOR_L = {
    6,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    SOME_BINARY_SENSOR_L_SENSOR_READINGS    // mSensorReadings
};

SensorData SOME_BINARY_SENSOR_R = {
    7,                                      // mSensorID
    CONNECTED_WORKING,                      // mSensorStatus
    1,                                      // mNumReadings
    SOME_BINARY_SENSOR_R_SENSOR_READINGS    // mSensorReadings
};


// Container for the fake sensors/descriptions
const int NUM_FAKE_SENSORS = 8;
SensorDescription * FAKE_SENSOR_DESCRIPTIONS[] = {
    &MOISTURE_SENSOR_L_DESCRIPTION,
    &MOISTURE_SENSOR_R_DESCRIPTION,
    &DHT22_SENSOR_L_DESCRIPTION,
    &DHT22_SENSOR_R_DESCRIPTION,
    &WEIGHT_SENSOR_L_DESCRIPTION,
    &WEIGHT_SENSOR_R_DESCRIPTION,
    &SOME_BINARY_SENSOR_L_DESCRIPTION,
    &SOME_BINARY_SENSOR_R_DESCRIPTION
};


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
