#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <Arduino.h>


#ifdef __cplusplus
extern "C"{
#endif


/**
 *              +--------------------------------------+
 *              | MSGPACK SENSOR DATA PACKET STRUCTURE |
 *              +--------------------------------------+
 * 
 *      {
 *          "sensor_id" : 0,                                    <- Sensor identifier. Unique, unsigned 8-bit
 *          "sensor_description" : "Sensor Description",        <- Packet description
 *          "sensor_status" : 0,                                <- Sensor status, see "SensorStatus" enum below for values. Unsigned 8-bit
 *          "sensor_readings" : [                               <- Individual sensor readings array
 *              {
 *                  "reading_name" : "First sensor reading",    <- Name of sensor
 *                  "reading_value" : 126.5                     <- Actual sensor value, can be float, bool (unsigned 8-bit int) or 16-bit int
 *                  "reading_details" : {                       <- Sensor value description
 *                      "type" : "float",                       <- Type of sensor value, see "ReadingType" enum below for values. Unsigned 8-bit
 *                      "min_value" : 0,                        <- Minimum sensor value
 *                      "max_value" : 256                       <- Maximum sensor value
 *                  }
 *              },
 *              {                                               <- Repeats for each individual sensor reading...
 *                  "reading_name" : "Second sensor reading",
 *                  "reading_value" : 12
 *                  "reading_details" : {
 *                      "type" : "integer",
 *                      "min_value" : 0,
 *                      "max_value" : 15
 *                  }
 *              }
 *          ]
 *      }
 * 
 */


// Type of reading a sensor will produce
typedef enum {
    INT_READING             = 0x01,         // 16-bit integer
    FLOAT_READING           = 0x02,         // Floating point value
    BOOL_READING            = 0x03          // On/Off value (sent as unsigned 8-bit integer)
} ReadingType;

// Describes the current sensor stats
typedef enum {
    CONNECTED_WORKING       = 0x00,
    CONNECTED_NOT_WORKING   = 0x01,
    DISCONNECTED            = 0x02
} SensorStatus;

// Union containing the actual underlying reading value
typedef union {
    uint16_t mIntValue;
    float mFloatValue;   
    uint8_t mBoolValue; 
} ReadingValue;

// Description of the sensor reading output (metadata - not an actual reading)
typedef struct {
    ReadingType mType;                      // The type of reading this sensor provides (i.e. int, float)
    ReadingValue mMinValue;                 // Minimum value in the range of readings for this sensor
    ReadingValue mMaxValue;                 // Maximum value in the range of readings for this sensor
} ReadingDetails;

// The actual sensor reading
typedef struct {
    const char *mSensorName;                // Name of sensor
    ReadingValue mValue;                    // Actual sensor reading
    ReadingDetails mDetails;                // Sensor reading description
} SensorReading;

// Data returned from  single piece of hardware which contains one or more individual sensors
typedef struct {
    uint8_t mSensorID;                      // Unique sensor identification value
    const char *mSensorDescription;         // Sensor pack description (i.e. name)
    SensorStatus mStatus;                   // Current status of sensor
    int mNumReadings;                       // How many individual reading types this sensor provides (i.e. size of below array)
    SensorReading mSensorReadings[];        // Each individual sensor reading
} SensorData;

// msgpack packing status response
typedef struct {        
    size_t mBytesUsed;                      // Number of bytes actually used by packing the data
    int mErrorCode;                         // Any underlying mpack error code returned during packing (0 = no error)
} PackResponse;


// Pack the supplied sensor data into the supplied buffer
PackResponse pack_sensor_data(SensorData *sensorData, uint8_t* outBuf, size_t outBufSize);


#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __SENSOR_H__
