#include "sensor_definitions.h" 

// I2C bus controller
I2CInterface sensorI2CInterface = {
    .mI2C = SENSOR_I2C,
    .mBaud = SENSOR_I2C_BAUDRATE,
    .mSDA = SENSOR_I2C_SDA,
    .mSCL = SENSOR_I2C_SCL,
    .mMultiplexerAddress = DEFAULT_MULTIPLEXER_ADDRESS,
    .mChannelConnectRegister = {
        .mDataPin = PISO_DATA_PIN,
        .mLatchPin = PISO_LATCH_PIN,
        .mClockPin = PISO_CLOCK_PIN,
        .mType = PISO_SHIFT_REGISTER
    }
};

SonarPIOWrapper PIO_WRAPPER = {
    .mPIO = pio0,
    .mInitialized = false
};

Sensor sensorsList[NUM_SENSORS] = {
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSonarSensor = {
                    .mTXPin = SONAR_SENSOR_TX_L1,
                    .mRXPin = SONAR_SENSOR_RX_L1,
                    .mJackDetectPin = SONAR_SENSOR_L1_JACK_DETECT_PIN,
                    .mBaudrate = SONAR_SENSOR_BAUDRATE,
                    .mStateMachineID = 0,
                    .mPIOWrapper = &PIO_WRAPPER
                }
            },
            .mSensorType = SONAR_SENSOR,
            .mSensorID = SONAR_SENSOR_L1_ID,
            .mSensorConnectLEDPosition = SONAR_SENSOR_L1_ACTIVE_LED
        }
    },
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSonarSensor = {
                    .mTXPin = SONAR_SENSOR_TX_R1,
                    .mRXPin = SONAR_SENSOR_RX_R1,
                    .mJackDetectPin = SONAR_SENSOR_R1_JACK_DETECT_PIN,
                    .mBaudrate = SONAR_SENSOR_BAUDRATE,
                    .mStateMachineID = 1,
                    .mPIOWrapper = &PIO_WRAPPER
                }
            },
            .mSensorType = SONAR_SENSOR,
            .mSensorID = SONAR_SENSOR_R1_ID,
            .mSensorConnectLEDPosition = SONAR_SENSOR_R1_ACTIVE_LED
        }
    },
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSensorPod = {
                    .mInterface = &sensorI2CInterface,
                    .mI2CChannel = I2C_CHANNEL_0,
                    .mSCD30Address = SCD30_I2C_ADDRESS,
                    .mSoilSensorAddress = SOIL_SENSOR_3_ADDRESS,
                }
            },
            .mSensorType = SENSOR_POD,
            .mSensorID = SENSOR_POD_L_ID,
            .mSensorConnectLEDPosition = SENSOR_POD_L_ACITVE_LED
        }
    },
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSensorPod = {
                    .mInterface = &sensorI2CInterface,
                    .mI2CChannel = I2C_CHANNEL_7,
                    .mSCD30Address = SCD30_I2C_ADDRESS,
                    .mSoilSensorAddress = SOIL_SENSOR_3_ADDRESS,
                }
            },
            .mSensorType = SENSOR_POD,
            .mSensorID = SENSOR_POD_R_ID,
            .mSensorConnectLEDPosition = SENSOR_POD_R_ACITVE_LED
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

MsgPackSensorReadingDescription MPACK_CO2_READING_DESCRIPTION = {
    0,                                      // mReadingID
    "CO2 (PPM)",                            // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=400.0f},                  // mMinValue
    {.mFloatValue=4000.0f}                  // mMaxValue
};

MsgPackSensorReadingDescription MPACK_TEMPERATURE_READING_DESCRIPTION = {
    1,                                      // mReadingID
    "Temperature (C)",                      // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=10.0f},                   // mMinValue
    {.mFloatValue=65.0f}                    // mMaxValue
};

MsgPackSensorReadingDescription MPACK_HUMIDITY_READING_DESCRIPTION = {
    2,                                      // mReadingID
    "RH (%)",                               // mReadingName
    FLOAT_READING,                          // mType   
    {.mFloatValue=0.0f},                    // mMinValue
    {.mFloatValue=100.0f}                   // mMaxValue
};

MsgPackSensorReadingDescription MPACK_SOIL_MOISTURE_READING_DESCRIPTION = {
    3,
    "Moisture level",                       // mReadingName
    INT_READING,                            // mType   
    {.mIntValue=0},                         // mMinValue
    {.mIntValue=2000}                       // mMaxValue
};


                        /////////////////////////////////////////////////
                        // Sensor data wrappers for connected hardware //
                        /////////////////////////////////////////////////

MsgPackSensorPacket sensorPackets[NUM_SENSORS] = {
    {
        .mSensorID = SONAR_SENSOR_L1_ID,
        .mSensorName = "Sonar Sensor L1",
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
            .mSensorReadings = (MsgPackSensorReading[1]) {
                {
                    .mDescription = &MPACK_SONAR_READING_DESCRIPTION,
                    .mValue = {
                        .mIntValue=0
                    }
                }    
            }
        }
    },
    {
        .mSensorID = SONAR_SENSOR_R1_ID,
        .mSensorName = "Sonar Sensor R1",
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
            .mSensorReadings = (MsgPackSensorReading[1]) {
                {
                    .mDescription = &MPACK_SONAR_READING_DESCRIPTION,
                    .mValue = {
                        .mIntValue=0
                    }
                }    
            }
        }
    },
    {
        .mSensorID = SENSOR_POD_L_ID,
        .mSensorName = "Sensor Pod L",
        .mSensorType = SENSOR_POD,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 4,
            .mSensorReadings = (MsgPackSensorReading[4]) {
                {
                    .mDescription = &MPACK_CO2_READING_DESCRIPTION,
                    .mValue = {
                        .mFloatValue = 0.f
                    }
                },
                {
                    .mDescription = &MPACK_TEMPERATURE_READING_DESCRIPTION,
                    .mValue = {
                        .mFloatValue=0.f
                    }
                },
                {
                    .mDescription = &MPACK_HUMIDITY_READING_DESCRIPTION,
                    .mValue = {
                        .mFloatValue=0.f
                    }
                },
                {
                    .mDescription = &MPACK_SOIL_MOISTURE_READING_DESCRIPTION,
                    .mValue = {
                        .mIntValue=0
                    }
                },
            }
        }
    },
    {
        .mSensorID = SENSOR_POD_R_ID,
        .mSensorName = "Sensor Pod R",
        .mSensorType = SENSOR_POD,
        .mCalibrationParams = {
            .mIsCalibratable = false,
            .mCalibrationValueType = FLOAT_READING,
            .mCalibrationRangeMin = {.mFloatValue=0.f},
            .mCalibrationRangeMax = {.mFloatValue=50.f}
        },
        .mCurrentSensorData = {
            .mStatus = SENSOR_DISCONNECTED,
            .mNumReadings = 4,
            .mSensorReadings = (MsgPackSensorReading[4]) {
                {
                    .mDescription = &MPACK_CO2_READING_DESCRIPTION,
                    .mValue = {
                        .mFloatValue = 0.f
                    }
                },
                {
                    .mDescription = &MPACK_TEMPERATURE_READING_DESCRIPTION,
                    .mValue = {
                        .mFloatValue=0.f
                    }
                },
                {
                    .mDescription = &MPACK_HUMIDITY_READING_DESCRIPTION,
                    .mValue = {
                        .mFloatValue=0.f
                    }
                },
                {
                    .mDescription = &MPACK_SOIL_MOISTURE_READING_DESCRIPTION,
                    .mValue = {
                        .mIntValue=0
                    }
                },
            }
        }
    }
};
