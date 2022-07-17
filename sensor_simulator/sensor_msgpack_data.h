#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <Arduino.h>


#ifdef __cplusplus
extern "C"{
#endif


/**
 *              +---------------------------+
 *              | MSGPACK PACKET STRUCTURES |
 *              +---------------------------+
 *              
 *              
 *      // Header packet
 *      {
 *          "packet_id" : 0,                                    <- Packet type identifier. See "PacketIdentifier" below for values. Unsigned 8-bit
 *          "response_code" : 0,                                <- Command response code. See "CommandResponseCode" below for values. Unsigned 8-bit
 *          "sensor_data_count" : 1                             <- Number of sensor data packets to follow. Unsigned 8-bit
 *      }
 * 
 *      // Sensor data packet
 *      {
 *          "packet_id" : 1,                                    <- Packet type identifier. Set to SENSOR_DATA for this packet
 *          "sensor_id" : 0,                                    <- Sensor identifier. Unique, unsigned 8-bit
 *          "sensor_status" : 0,                                <- Sensor status, see "SensorStatus" enum below for values. Unsigned 8-bit
 *          "sensor_readings" : [                               <- Individual sensor readings array
 *              "value" : 126.5                                 <- Actual sensor readings, can be float, bool (unsigned 8-bit int) or 16-bit int. 
 *              "value" : 12
 *          ]
 *      }
 *      
 *      // Sensor description packet
 *      {
 *          "packet_id" : 2,                                    <- Packet type identifier. Set to SENSOR_DESCRIPTION_PACKET for this packet
 *          "sensor_id" : 0,                                    <- Sensor identifier. Unique, unsigned 8-bit
 *          "name" : "Sensor Description",                      <- Sensor description/name
 *          "reading_details" : [                               <- Individual sensor reading descriptions array
 *              {
 *                  "name" : "First sensor reading",            <- Name of sensor
 *                  "type" : "float",                           <- Type of sensor value, see "ReadingType" enum below for values. Unsigned 8-bit
 *                  "min_value" : 0.0,                          <- Minimum sensor value
 *                  "max_value" : 25.5                          <- Maximum sensor value
 *                  }
 *              },
 *              {                                               <- Repeats for each individual sensor reading...
 *                  "name" : "Second sensor reading",
 *                  "type" : "integer",
 *                  "min_value" : 0,
 *                  "max_value" : 15
 *                  }
 *              }
 *          ]
 *      }
 *      
 *      // Terminator packet
 *      {
 *          "packet_id" : 255,                                  <- Packet type identifier. Set to TERMINATOR for this packet
 *      }
 * 
 */


// Type of packet we are sending
typedef enum {
    COMMAND_RESPONSE_PACKET     = 0x00,
    SENSOR_DATA_PACKET          = 0x01,
    SENSOR_DESCRIPTION_PACKET   = 0x02,
    TERMINATOR_PACKET           = 0xFF
} PacketIdentifier;


// Command response codes 
typedef enum {
    COMMAND_OK                  = 0x00,
    SENSOR_NOT_FOUND            = 0x01
} CommandResponseCode;


// Describes the current sensor stats
typedef enum {
    CONNECTED_WORKING           = 0x00,
    CONNECTED_NOT_WORKING       = 0x01,
    DISCONNECTED                = 0x02
} SensorStatus;


// Type of reading a sensor will produce
typedef enum {
    INT_READING                 = 0x01,         // 16-bit integer
    FLOAT_READING               = 0x02,         // Floating point value
    BOOL_READING                = 0x03          // On/Off value (sent as unsigned 8-bit integer)
} ReadingType;


// Union containing the actual underlying reading value
typedef union {
    uint16_t mIntValue;
    float mFloatValue;   
    uint8_t mBoolValue; 
} ReadingValue;


typedef struct {
    ReadingType mType;
    ReadingValue mValue;    
} SensorReading;


// Description of the sensor reading output (metadata - not an actual reading)
typedef struct {
    const char *mReadingName;               // Name of reading
    ReadingType mType;                      // The type of reading this sensor provides (i.e. int, float)
    ReadingValue mMinValue;                 // Minimum value in the range of readings for this sensor
    ReadingValue mMaxValue;                 // Maximum value in the range of readings for this sensor
} ReadingDetails;


// 
typedef struct {
    uint8_t mSensorID;                      // Unique sensor identification value
    const char *mSensorName;                // Sensor/description name
    int mNumReadings;                       // How many individual reading types this sensor provides (i.e. size of below array)
    ReadingDetails **mReadingDetails;      // Details on each individual sensor reading
} SensorDescription;


// Data returned from  single piece of hardware which contains one or more individual sensors
typedef struct {
    uint8_t mSensorID;                      // Unique sensor identification value
    SensorStatus mStatus;                   // Current status of sensor
    int mNumReadings;                       // How many individual reading types this sensor provides (i.e. size of below array)
    SensorReading *mSensorReadings;         // Each individual sensor reading
} SensorData;


typedef struct {
    CommandResponseCode mResponseCode;      // The response code for the issued command
    uint8_t mSensorDataCount;               // How many sensor data packets will follow
} HeaderPacket;


// msgpack packing status response
typedef struct {        
    size_t mBytesUsed;                      // Number of bytes actually used by packing the data
    int mErrorCode;                         // Any underlying mpack error code returned during packing (0 = no error - see mpack_error_t in mpack.h for more values)
} PackResponse;


// Pack the supplied sensor data into the supplied buffer
PackResponse pack_header_data(HeaderPacket headerPacket, char* outBuf, size_t outBufSize);
PackResponse pack_terminator_packet(char* outBuf, size_t outBufSize);
PackResponse pack_sensor_data(SensorData *sensorData, char* outBuf, size_t outBufSize);
PackResponse pack_sensor_description(SensorDescription *sensorDescription, char* outBuf, size_t outBufSize);


#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __SENSOR_H__
