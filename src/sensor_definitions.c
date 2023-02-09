#include "sensor_definitions.h" 

SonarPIOWrapper PIO_WRAPPER = {
    .mPIO = pio0,
    .mInitialized = false
};

Sensor sensorsList[NUM_SENSORS] = {
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
    },
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    }
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

MsgPackSensorPacket sensorPackets[NUM_SENSORS] = {
    {
        .mSensorID = SONAR_SENSOR_L_ID,
        .mSensorName = "Sonar Sensor L",
        .mSensorType = SONAR_SENSOR,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 1,
            .mSensorReadings = MPACK_SONAR_SENSOR_L_READINGS
        }
    },
    {
        .mSensorID = SONAR_SENSOR_R_ID,
        .mSensorName = "Sonar Sensor R",
        .mSensorType = SONAR_SENSOR,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 1,
            .mSensorReadings = MPACK_SONAR_SENSOR_R_READINGS
        }
    },
    {
        .mSensorID = MOISTURE_SENSOR_L_ID,
        .mSensorName = "Moisture Sensor L",
        .mSensorType = MOISTURE_SENSOR,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 1,
            .mSensorReadings = MPACK_MOISTURE_SENSOR_L_READINGS
        }
    },
    {
        .mSensorID = MOISTURE_SENSOR_R_ID,
        .mSensorName = "Moisture Sensor R",
        .mSensorType = MOISTURE_SENSOR,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 1,
            .mSensorReadings = MPACK_MOISTURE_SENSOR_R_READINGS
        }
    },
    {
        .mSensorID = TEMP_SENSOR_L_ID,
        .mSensorName = "Temp/Humidity Sensor L",
        .mSensorType = TEMP_HUMIDITY_SENSOR,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 2,
            .mSensorReadings = MPACK_DHT22_SENSOR_L_READINGS
        }
    },
    {
        .mSensorID = TEMP_SENSOR_R_ID,
        .mSensorName = "Temp/Humidity Sensor R",
        .mSensorType = TEMP_HUMIDITY_SENSOR,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 2,
            .mSensorReadings = MPACK_DHT22_SENSOR_R_READINGS
        }
    }
};
