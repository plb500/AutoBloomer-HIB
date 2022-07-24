#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <Arduino.h>


#ifdef __cplusplus
extern "C"{
#endif


/**
 *              /------------------------------------\
 *              | OUTGOING MSGPACK PACKET STRUCTURES |
 *              \------------------------------------/
 *              
 *              
 *      // "Controller ready" packet
 *      {
 *          "packet_id" : 254,                                  <- Packet type identifier. See "PacketIdentifier" below for values. Unsigned 8-bit
 *      }
 *              
 *      // Header packet
 *      {
 *          "packet_id" : 0,                                    <- Packet type identifier. See "PacketIdentifier" below for values. Unsigned 8-bit
 *          "response_code" : 0,                                <- Command response code. See "CommandResponseCode" below for values. Unsigned 8-bit
 *          "sensor_data_count" : 1                             <- Number of sensor data packets to follow. Unsigned 8-bit
 *      }
 *      
 *      // Sensor reading description object
 *      {
 *          "name" : "Reading name",
 *          "type" : 0,
 *          "min_value" : 0.0,
 *          "max_value" : 25.5,
 *      }
 *      
 *      // Sensor reading object
 *      {
 *          "description" : <Sensor reading description object> 
 *          "value" : 12.3      
 *      }
 *      
 *      // Sensor data packet
 *      {
 *          "packet_id" : 1,                                    <- Packet type identifier. Set to SENSOR_DATA for this packet
 *          "sensor_id" : 0,                                    <- Sensor identifier. Unique, unsigned 8-bit
 *          "name" : "DHT22 Temp/RH sensor"                     <- Sensor name
 *          "sensor_status" : 0,                                <- Sensor status, see "SensorStatus" enum below for values. Unsigned 8-bit
 *          "sensor_readings" : [                               <- Individual sensor readings array
 *              <Sensor Reading object>,                        <- See "Sensor reading object" above
 *              <Sensor Reading object>
 *              ....
 *          ]
 *      }
 *      
 *      // Terminator packet
 *      {
 *          "packet_id" : 255,                                  <- Packet type identifier. Set to TERMINATOR for this packet
 *      }
 * 
 */

// Sensor commands
typedef enum { 
    NO_COMMAND                  = 0x00,
    GET_ALL_SENSOR_VALUES       = 0x01,
    GET_SENSOR_VALUE            = 0x02,
} SensorCommandIdentifier;


// Command response codes 
typedef enum {
    COMMAND_OK                  = 0x00,
    SENSOR_NOT_FOUND            = 0x01,

    CONTROLLER_READY            = 0xFF
} CommandResponseCode;


// Type of packet we are sending
typedef enum {
    HEADER_PACKET               = 0x00,
    SENSOR_DATA_PACKET          = 0x01,
    SENSOR_DESCRIPTION_PACKET   = 0x02,
    CONTROLLER_READY_PACKET     = 0xFE,
    TERMINATOR_PACKET           = 0xFF
} PacketIdentifier;


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


// Description of the sensor reading output (metadata - not an actual reading)
typedef struct {
    const char *mReadingName;               // Name of reading
    ReadingType mType;                      // The type of reading this sensor provides (i.e. int, float)
    ReadingValue mMinValue;                 // Minimum value in the range of readings for this sensor
    ReadingValue mMaxValue;                 // Maximum value in the range of readings for this sensor
} SensorReadingDescription;


// Container for an individual reading value. Also contains a description of the reading
typedef struct {
    const SensorReadingDescription* const mDescription;
    ReadingValue mValue;    
} SensorReading;


typedef struct {
    uint8_t mSensorID;                      // Unique sensor identification value
    const char *mSensorName;                // Sensor name/description
    SensorStatus mStatus;                   // Current status of sensor
    int mNumReadings;                       // How many individual reading types this sensor provides (i.e. size of below array)
    SensorReading *mSensorReadings;         // Each individual sensor reading
} SensorData;


typedef struct {
    SensorCommandIdentifier mCommandID;     // The command we are responding to
    CommandResponseCode mResponseCode;      // The response code for the issued command
} HeaderPacket;


// msgpack packing status response
typedef struct {        
    size_t mBytesUsed;                      // Number of bytes actually used by packing the data
    int mErrorCode;                         // Any underlying mpack error code returned during packing (0 = no error - see mpack_error_t in mpack.h for more values)
} PackResponse;


// Packs a response indicating sensor controller is now ready for comms
PackResponse pack_controller_ready_packet(char* outBuf, size_t outBufSize);

// Packs the header for a response to an issued command
PackResponse pack_header_data(HeaderPacket headerPacket, char* outBuf, size_t outBufSize);

// Packs the packet which signifies and end of response
PackResponse pack_terminator_packet(uint8_t terminatorCode, char* outBuf, size_t outBufSize);

// Packs a data packet for a single sensor
PackResponse pack_sensor_data(const SensorData * const sensorData, char* outBuf, size_t outBufSize);


#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __SENSOR_H__
