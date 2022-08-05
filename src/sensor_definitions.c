#include "sensor_definitions.h" 

Sensor LOAD_SENSOR_L_HW = {
    {
        .mHX711 = {
            LOAD_SENSOR_SCL_L,
            LOAD_SENSOR_SDA_L,
            GAIN_FACTOR_128,
            0,
            2491.467f,
        }
    },
    LOAD_SENSOR,
    LOAD_SENSOR_L_ID,
    false,
    LOAD_SENSOR_L_JACK_DETECT_PIN
};

Sensor LOAD_SENSOR_R_HW = {
    {
        .mHX711 = {
            LOAD_SENSOR_SCL_R,
            LOAD_SENSOR_SDA_R,
            GAIN_FACTOR_128,
            0,
            2491.467f,
        }
    },
    LOAD_SENSOR,
    LOAD_SENSOR_R_ID,
    false,
    LOAD_SENSOR_R_JACK_DETECT_PIN
};

Sensor TEMP_SENSOR_L_HW = {
    {
        .mDHTSensor = {
            TEMP_SENSOR_L_CTL,
            0
        }
    },
    TEMP_HUMIDITY_SENSOR,
    TEMP_SENSOR_L_ID,
    false,
    TEMP_SENSOR_L_JACK_DETECT_PIN
};

Sensor TEMP_SENSOR_R_HW = {
    {
        .mDHTSensor = {
            TEMP_SENSOR_R_CTL,
            0
        }
    },
    TEMP_HUMIDITY_SENSOR,
    TEMP_SENSOR_R_ID,
    false,
    TEMP_SENSOR_R_JACK_DETECT_PIN
};

Sensor MOISTURE_SENSOR_L_HW = {
    {
        .mMoistureSensor = {
            SENSOR_I2C, 
            SOIL_SENSOR_1_ADDRESS
        }
    },
    MOISTURE_SENSOR,
    MOISTURE_SENSOR_L_ID,
    false,
    MOISTURE_SENSOR_L_JACK_DETECT_PIN
};

Sensor MOISTURE_SENSOR_R_HW = {
    {
        .mMoistureSensor = {
            SENSOR_I2C, 
            SOIL_SENSOR_1_ADDRESS
        }
    },
    MOISTURE_SENSOR,
    MOISTURE_SENSOR_R_ID,
    false,
    MOISTURE_SENSOR_R_JACK_DETECT_PIN
};


// Our list of actual sensor hardware
Sensor* _sensorsList[NUM_SENSORS] = {
    &LOAD_SENSOR_L_HW,
    &LOAD_SENSOR_R_HW,
    &TEMP_SENSOR_L_HW,
    &TEMP_SENSOR_R_HW,
    &MOISTURE_SENSOR_L_HW,
    &MOISTURE_SENSOR_R_HW
};

                        ///////////////////////////////////////////////////////
                        // Sensor output descriptions for connected hardware //
                        ///////////////////////////////////////////////////////

// Static reading definitions
MsgPackSensorReadingDescription MPACK_TEMPERATURE_READING_DESCRIPTION = {
    "Temperature (C)",                      // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=10.0f},                   // mMinValue
    {.mFloatValue=65.0f}                    // mMaxValue
};

MsgPackSensorReadingDescription MPACK_HUMIDITY_READING_DESCRIPTION = {
    "RH (%)",                               // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=100.0f}                   // mMaxValue
};

MsgPackSensorReadingDescription MPACK_SOIL_MOISTURE_READING_DESCRIPTION = {
    "Moisture level",                       // mReadingName,
    INT_READING,                            // mType   
    {.mIntValue=0},                         // mMinValue
    {.mIntValue=500}                        // mMaxValue
};

MsgPackSensorReadingDescription MPACK_WEIGHT_READING_DESCRIPTION = {
    "Weight (kg)",                          // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=20.0f}                    // mMaxValue
};


// Reading output objects
MsgPackSensorReading MPACK_MOISTURE_SENSOR_L_READINGS[] = {
    {
        &MPACK_SOIL_MOISTURE_READING_DESCRIPTION,
        {.mIntValue=0}
    }    
};

MsgPackSensorReading MPACK_MOISTURE_SENSOR_R_READINGS[] = {
    {
        &MPACK_SOIL_MOISTURE_READING_DESCRIPTION,
        {.mIntValue=0}
    }    
};

MsgPackSensorReading MPACK_DHT22_SENSOR_L_READINGS[] = {
    {
        &MPACK_TEMPERATURE_READING_DESCRIPTION,
        {.mFloatValue=0.f}
    },
    {
        &MPACK_HUMIDITY_READING_DESCRIPTION,
        {.mFloatValue=0.f}
    }
};

MsgPackSensorReading MPACK_DHT22_SENSOR_R_READINGS[] = {
    {
        &MPACK_TEMPERATURE_READING_DESCRIPTION,
        {.mFloatValue=0.f}
    },
    {
        &MPACK_HUMIDITY_READING_DESCRIPTION,
        {.mFloatValue=0.f}
    }
};

MsgPackSensorReading MPACK_WEIGHT_SENSOR_L_READINGS[] = {
    {
        &MPACK_WEIGHT_READING_DESCRIPTION,
        {.mFloatValue=0.f}
    }
};

MsgPackSensorReading MPACK_WEIGHT_SENSOR_R_READINGS[] = {
    {
        &MPACK_WEIGHT_READING_DESCRIPTION,
        {.mFloatValue=0.f}
    }
};

                        /////////////////////////////////////////////////
                        // Sensor data wrappers for connected hardware //
                        /////////////////////////////////////////////////

MsgPackSensorData MPACK_MOISTURE_SENSOR_L = {
    MOISTURE_SENSOR_L_ID,                   // mSensorID
    "Mother moisture sensor",               // mSensorName
    DISCONNECTED,                           // mSensorStatus
    1,                                      // mNumReadings
   MPACK_MOISTURE_SENSOR_L_READINGS         // Each individual sensor reading
};

MsgPackSensorData MPACK_MOISTURE_SENSOR_R = {
    MOISTURE_SENSOR_R_ID,                   // mSensorID
    "Right moisture sensor",                // mSensorName
    DISCONNECTED,                           // mSensorStatus
    1,                                      // mNumReadings
    MPACK_MOISTURE_SENSOR_R_READINGS        // Each individual sensor reading
};

MsgPackSensorData MPACK_DHT22_SENSOR_L = {
    TEMP_SENSOR_L_ID,                       // mSensorID
    "Left DHT22 Temp/Humidity sensor",      // mSensorName
    DISCONNECTED,                           // mSensorStatus
    2,                                      // mNumReadings
    MPACK_DHT22_SENSOR_L_READINGS           // mReadingDetails
};

MsgPackSensorData MPACK_DHT22_SENSOR_R = {
    TEMP_SENSOR_R_ID,                       // mSensorID
    "Right DHT22 Temp/Humidity sensor",     // mSensorName
    DISCONNECTED,                           // mSensorStatus
    2,                                      // mNumReadings
    MPACK_DHT22_SENSOR_R_READINGS           // mReadingDetails
};

MsgPackSensorData MPACK_WEIGHT_SENSOR_L = {
    LOAD_SENSOR_L_ID,                       // mSensorID
    "Left weight sensor",                   // mSensorName
    DISCONNECTED,                           // mSensorStatus
    1,                                      // mNumReadings
    MPACK_WEIGHT_SENSOR_L_READINGS          // mReadingDetails
};

MsgPackSensorData MPACK_WEIGHT_SENSOR_R = {
    LOAD_SENSOR_R_ID,                       // mSensorID
    "Right weight sensor",                  // mSensorName
    DISCONNECTED,                           // mSensorStatus
    1,                                      // mNumReadings
    MPACK_WEIGHT_SENSOR_R_READINGS          // mReadingDetails
};


// Container for the fake sensors/descriptions
MsgPackSensorData * SENSOR_MSGPACK[NUM_SENSORS] = {
    &MPACK_WEIGHT_SENSOR_L,
    &MPACK_WEIGHT_SENSOR_R,
    &MPACK_MOISTURE_SENSOR_L,
    &MPACK_MOISTURE_SENSOR_R,
    &MPACK_DHT22_SENSOR_L,
    &MPACK_DHT22_SENSOR_R
};