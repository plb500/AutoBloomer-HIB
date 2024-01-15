#include "sensor_definitions.h" 

const char * const FEED_SENSOR_NAME = "FeedSensor";
const char * const SENSOR_POD_NAME = "SensorPod";
const char * const BATTERY_SENSOR_NAME = "BatteryMonitor";

const char * const HARVEST_CHAMBER_LOCATION = "HarvestChamber";
const char * const MOTHER_CHAMBER_LOCATION = "MotherChamber";
const char * const HIB_LOCATION = "HIB";


I2CMultiplexer sensorI2CMultiplexer = {
    .mMultiplexerAddress        = DEFAULT_MULTIPLEXER_ADDRESS,
    .mResetPin                  = SENSOR_I2C_MULTIPLEXER_RESET_PIN,
};

// I2C bus controller
I2CInterface sensorI2CInterface = {
    .mI2C = SENSOR_I2C,
    .mBaud = SENSOR_I2C_BAUDRATE,
    .mSDA = SENSOR_I2C_SDA_PIN,
    .mSCL = SENSOR_I2C_SCL_PIN,
    .mMultiplexer = &sensorI2CMultiplexer
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
                    .mTXPin = SONAR_SENSOR_L1_TX_PIN,
                    .mRXPin = SONAR_SENSOR_L1_RX_PIN,
                    .mBaudrate = SONAR_SENSOR_BAUDRATE,
                    .mStateMachineID = 0,
                    .mPIOWrapper = &PIO_WRAPPER
                }
            },
            .mSensorType = SONAR_SENSOR,
            .mSensorID = SONAR_SENSOR_L1_ID,
            .mSensorName = FEED_SENSOR_NAME,
            .mSensorDisplayName = "Harvest Chamber Feed Sensor",
            .mSensorLocation = HARVEST_CHAMBER_LOCATION,
            .mSensorConnectLEDPosition = SONAR_SENSOR_L1_ACTIVE_LED,
            .mHardwareConnectionID = FEED_SENSOR_L1_CONNECT_ID
        }
    },
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSonarSensor = {
                    .mTXPin = SONAR_SENSOR_R1_TX_PIN,
                    .mRXPin = SONAR_SENSOR_R1_RX_PIN,
                    .mBaudrate = SONAR_SENSOR_BAUDRATE,
                    .mStateMachineID = 1,
                    .mPIOWrapper = &PIO_WRAPPER
                }
            },
            .mSensorType = SONAR_SENSOR,
            .mSensorID = SONAR_SENSOR_R1_ID,
            .mSensorName = FEED_SENSOR_NAME,
            .mSensorDisplayName = "Mother Chamber Feed Sensor",
            .mSensorLocation = MOTHER_CHAMBER_LOCATION,
            .mSensorConnectLEDPosition = SONAR_SENSOR_R1_ACTIVE_LED,
            .mHardwareConnectionID = FEED_SENSOR_R1_CONNECT_ID
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
            .mSensorDisplayName = "Harvest Chamber Sensor Pod",
            .mSensorName = SENSOR_POD_NAME,
            .mSensorLocation = HARVEST_CHAMBER_LOCATION,
            .mSensorConnectLEDPosition = SENSOR_POD_L_ACTIVE_LED,
            .mHardwareConnectionID = I2C_DEVICE_0_CONNECT_ID
        }
    },
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mSensorPod = {
                    .mInterface = &sensorI2CInterface,
                    .mI2CChannel = I2C_CHANNEL_7,
                    .mSCD30Address = SCD30_I2C_ADDRESS,
                    .mSoilSensorAddress = SOIL_SENSOR_1_ADDRESS,
                }
            },
            .mSensorType = SENSOR_POD,
            .mSensorID = SENSOR_POD_R_ID,
            .mSensorName = SENSOR_POD_NAME,
            .mSensorDisplayName = "Mother Chamber Sensor Pod",
            .mSensorLocation = MOTHER_CHAMBER_LOCATION,
            .mSensorConnectLEDPosition = SENSOR_POD_R_ACTIVE_LED,
            .mHardwareConnectionID = I2C_DEVICE_7_CONNECT_ID
        }
    },
    {
        .mSensorDefinition = {                               
            .mSensor = {
                .mBatterySensor = {
                    .mEnableSensePin = BATTERY_SENSE_ENABLE_PIN,
                    .mBatteryMeasurePin = BATTERY_SENSE_PIN,
                    .mADCInput = RTC_BATTERY_ADC_PORT
                }
            },
            .mSensorType = BATTERY_SENSOR,
            .mSensorID = RTC_BATTERY_SENSOR,
            .mSensorName = BATTERY_SENSOR_NAME,
            .mSensorDisplayName = "Controller Clock Battery",
            .mSensorLocation = HIB_LOCATION,
            .mSensorConnectLEDPosition = NO_LED,
            .mHardwareConnectionID = ALWAYS_CONNECTED_CONNECT_ID
        }
    }
};
