#include "sensor_definitions.h" 

SonarPIOWrapper PIO_WRAPPER = {
    .mPIO = pio0,
    .mInitialized = false
};

Sensor maybeList[NUM_SENSORS] = {
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSonarSensor = {
                    .mTXPin = SONAR_SENSOR_TX_L,
                    .mRXPin = SONAR_SENSOR_RX_L,
                    .mBaudrate = SONAR_SENSOR_BAUDRATE,
                    .mStateMachineID = 0,
                    .mPIOWrapper = &PIO_WRAPPER
                }
            },
            .mSensorType = SONAR_SENSOR,
            .mSensorID = SONAR_SENSOR_L_ID,
            .mJackDetectPin = SONAR_SENSOR_L_JACK_DETECT_PIN,
            .mSensorConnectLEDPosition = SONAR_SENSOR_L_ACTIVE_LED
        }
    },
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSonarSensor = {
                    .mTXPin = SONAR_SENSOR_TX_R,
                    .mRXPin = SONAR_SENSOR_RX_R,
                    .mBaudrate = SONAR_SENSOR_BAUDRATE,
                    .mStateMachineID = 1,
                    .mPIOWrapper = &PIO_WRAPPER
                }
            },
            .mSensorType = SONAR_SENSOR,
            .mSensorID = SONAR_SENSOR_R_ID,
            .mJackDetectPin = SONAR_SENSOR_R_JACK_DETECT_PIN,
            .mSensorConnectLEDPosition = SONAR_SENSOR_R_ACTIVE_LED
        }
    }
};




Sensor SONAR_SENSOR_L_HW = {
    .mSensorDefinition = {                               
        .mSensor = {
            .mSonarSensor = {
                .mTXPin = SONAR_SENSOR_TX_L,
                .mRXPin = SONAR_SENSOR_RX_L,
                .mBaudrate = SONAR_SENSOR_BAUDRATE,
                .mStateMachineID = 0,
                .mPIOWrapper = &PIO_WRAPPER
            }
        },
        .mSensorType = SONAR_SENSOR,
        .mSensorID = SONAR_SENSOR_L_ID,
        .mJackDetectPin = SONAR_SENSOR_L_JACK_DETECT_PIN,
        .mSensorConnectLEDPosition = SONAR_SENSOR_L_ACTIVE_LED
    }
};

Sensor SONAR_SENSOR_R_HW = {
    .mSensorDefinition = {                               
        .mSensor = {
            .mSonarSensor = {
                .mTXPin = SONAR_SENSOR_TX_R,
                .mRXPin = SONAR_SENSOR_RX_R,
                .mBaudrate = SONAR_SENSOR_BAUDRATE,
                .mStateMachineID = 1,
                .mPIOWrapper = &PIO_WRAPPER
            }
        },
        .mSensorType = SONAR_SENSOR,
        .mSensorID = SONAR_SENSOR_R_ID,
        .mJackDetectPin = SONAR_SENSOR_R_JACK_DETECT_PIN,
        .mSensorConnectLEDPosition = SONAR_SENSOR_R_ACTIVE_LED
    }
};

Sensor TEMP_SENSOR_L_HW = {
    .mSensorDefinition = {                               
        .mSensor = {
            .mDHTSensor = {
                TEMP_SENSOR_L_CTL,
                0
            }
        },
        .mSensorType = TEMP_HUMIDITY_SENSOR,
        .mSensorID = TEMP_SENSOR_L_ID,
        .mJackDetectPin = TEMP_SENSOR_L_JACK_DETECT_PIN,
        .mSensorConnectLEDPosition = TEMP_SENSOR_L_ACTIVE_LED
    }
};

Sensor TEMP_SENSOR_R_HW = {
    .mSensorDefinition = {                               
        .mSensor = {
            .mDHTSensor = {
                TEMP_SENSOR_R_CTL,
                0
            }
        },
        .mSensorType = TEMP_HUMIDITY_SENSOR,
        .mSensorID = TEMP_SENSOR_R_ID,
        .mJackDetectPin = TEMP_SENSOR_R_JACK_DETECT_PIN,
        .mSensorConnectLEDPosition = TEMP_SENSOR_R_ACTIVE_LED
    }
};

Sensor MOISTURE_SENSOR_L_HW = {
    .mSensorDefinition = {                               
        .mSensor = {
            .mMoistureSensor = {
                SENSOR_I2C, 
                SOIL_SENSOR_3_ADDRESS
            }
        },
        .mSensorType = MOISTURE_SENSOR,
        .mSensorID = MOISTURE_SENSOR_L_ID,
        .mJackDetectPin = MOISTURE_SENSOR_L_JACK_DETECT_PIN,
        .mSensorConnectLEDPosition = MOISTURE_SENSOR_L_ACTIVE_LED
    }
};

Sensor MOISTURE_SENSOR_R_HW = {
    .mSensorDefinition = {                               
        .mSensor = {
            .mMoistureSensor = {
                SENSOR_I2C, 
                SOIL_SENSOR_3_ADDRESS
            }
        },
        .mSensorType = MOISTURE_SENSOR,
        .mSensorID = MOISTURE_SENSOR_R_ID,
        .mJackDetectPin = MOISTURE_SENSOR_R_JACK_DETECT_PIN,
        .mSensorConnectLEDPosition = MOISTURE_SENSOR_R_ACTIVE_LED
    }
};


// Our list of actual sensor hardware
Sensor* _sensorsList[NUM_SENSORS] = {
    &SONAR_SENSOR_L_HW,
    &SONAR_SENSOR_R_HW,
    &TEMP_SENSOR_L_HW,
    &TEMP_SENSOR_R_HW,
    &MOISTURE_SENSOR_L_HW,
    &MOISTURE_SENSOR_R_HW
};

                        ///////////////////////////////////////////////////////
                        // Sensor output descriptions for connected hardware //
                        ///////////////////////////////////////////////////////

// Static reading definitions
MsgPackSensorReadingDescription MPACK_SONAR_READING_DESCRIPTION = {
    0,                                      // mReadingID
    "Distance (mm)",                        // mReadingName
    INT_READING,                            // mType   
    {.mIntValue=30},                        // mMinValue
    {.mIntValue=4500}                       // mMaxValue
};

MsgPackSensorReadingDescription MPACK_TEMPERATURE_READING_DESCRIPTION = {
    0,                                      // mReadingID
    "Temperature (C)",                      // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=10.0f},                   // mMinValue
    {.mFloatValue=65.0f}                    // mMaxValue
};

MsgPackSensorReadingDescription MPACK_HUMIDITY_READING_DESCRIPTION = {
    1,                                      // mReadingID
    "RH (%)",                               // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=100.0f}                   // mMaxValue
};

MsgPackSensorReadingDescription MPACK_SOIL_MOISTURE_READING_DESCRIPTION = {
    0,
    "Moisture level",                       // mReadingName
    INT_READING,                            // mType   
    {.mIntValue=0},                         // mMinValue
    {.mIntValue=500}                        // mMaxValue
};


// Reading output objects
MsgPackSensorReading MPACK_SONAR_SENSOR_L_READINGS[] = {
    {
        &MPACK_SONAR_READING_DESCRIPTION,
        {.mIntValue=0}
    }    
};

MsgPackSensorReading MPACK_SONAR_SENSOR_R_READINGS[] = {
    {
        &MPACK_SONAR_READING_DESCRIPTION,
        {.mIntValue=0}
    }    
};

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

                        /////////////////////////////////////////////////
                        // Sensor data wrappers for connected hardware //
                        /////////////////////////////////////////////////
MsgPackSensorData MPACK_SONAR_SENSOR_L = {
    SONAR_SENSOR_L_ID,                      // mSensorID
    "Sonar Sensor L",                       // mSensorName
    SENSOR_DISCONNECTED,                    // mSensorStatus
    {                                       // mCalibration
        false,                              // mIsCalibratable
        FLOAT_READING,                      // mCalibrationValueType
        {.mFloatValue=0.f},                 // mCalibrationRangeMin
        {.mFloatValue=50.f}                 // mCalibrationRangeMax
    },
    1,                                      // mNumReadings
    MPACK_SONAR_SENSOR_L_READINGS           // Each individual sensor reading
};

MsgPackSensorData MPACK_SONAR_SENSOR_R = {
    SONAR_SENSOR_R_ID,                      // mSensorID
    "Sonar Sensor R",                       // mSensorName
    SENSOR_DISCONNECTED,                    // mSensorStatus
    {                                       // mCalibration
        false,                              // mIsCalibratable
        FLOAT_READING,                      // mCalibrationValueType
        {.mFloatValue=0.f},                 // mCalibrationRangeMin
        {.mFloatValue=50.f}                 // mCalibrationRangeMax
    },
    1,                                      // mNumReadings
    MPACK_SONAR_SENSOR_R_READINGS           // Each individual sensor reading
};

MsgPackSensorData MPACK_MOISTURE_SENSOR_L = {
    MOISTURE_SENSOR_L_ID,                   // mSensorID
    "Moisture Sensor L",                    // mSensorName
    SENSOR_DISCONNECTED,                    // mSensorStatus
    {                                       // mCalibration
        false,                              // mIsCalibratable
        FLOAT_READING,                      // mCalibrationValueType
        {.mFloatValue=0.f},                 // mCalibrationRangeMin
        {.mFloatValue=50.f}                 // mCalibrationRangeMax
    },
    1,                                      // mNumReadings
   MPACK_MOISTURE_SENSOR_L_READINGS         // Each individual sensor reading
};

MsgPackSensorData MPACK_MOISTURE_SENSOR_R = {
    MOISTURE_SENSOR_R_ID,                   // mSensorID
    "Moisture Sensor R",                    // mSensorName
    SENSOR_DISCONNECTED,                    // mSensorStatus
    {                                       // mCalibration
        false,                              // mIsCalibratable
        FLOAT_READING,                      // mCalibrationValueType
        {.mFloatValue=0.f},                 // mCalibrationRangeMin
        {.mFloatValue=50.f}                 // mCalibrationRangeMax
    },
    1,                                      // mNumReadings
    MPACK_MOISTURE_SENSOR_R_READINGS        // Each individual sensor reading
};

MsgPackSensorData MPACK_DHT22_SENSOR_L = {
    TEMP_SENSOR_L_ID,                       // mSensorID
    "Temp/Humidity Sensor L",               // mSensorName
    SENSOR_DISCONNECTED,                    // mSensorStatus
    {                                       // mCalibration
        false,                              // mIsCalibratable
        FLOAT_READING,                      // mCalibrationValueType
        {.mFloatValue=0.f},                 // mCalibrationRangeMin
        {.mFloatValue=50.f}                 // mCalibrationRangeMax
    },
    2,                                      // mNumReadings
    MPACK_DHT22_SENSOR_L_READINGS           // mReadingDetails
};

MsgPackSensorData MPACK_DHT22_SENSOR_R = {
    TEMP_SENSOR_R_ID,                       // mSensorID
    "Temp/Humidity Sensor R",               // mSensorName
    SENSOR_DISCONNECTED,                    // mSensorStatus
    {                                       // mCalibration
        false,                              // mIsCalibratable
        FLOAT_READING,                      // mCalibrationValueType
        {.mFloatValue=0.f},                 // mCalibrationRangeMin
        {.mFloatValue=50.f}                 // mCalibrationRangeMax
    },
    2,                                      // mNumReadings
    MPACK_DHT22_SENSOR_R_READINGS           // mReadingDetails
};


// Container for the fake sensors/descriptions
MsgPackSensorData * SENSOR_MSGPACK[NUM_SENSORS] = {
    &MPACK_SONAR_SENSOR_L,
    &MPACK_SONAR_SENSOR_R,
    &MPACK_MOISTURE_SENSOR_L,
    &MPACK_MOISTURE_SENSOR_R,
    &MPACK_DHT22_SENSOR_L,
    &MPACK_DHT22_SENSOR_R
};
