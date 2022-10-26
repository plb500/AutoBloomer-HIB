#ifndef SENSOR_MSGPACK_H
#define SENSOR_MSGPACK_H

#include <unistd.h>
#include <stdbool.h>
#include "sensor_controller/command_definitions.h"

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

// Type of packet we are sending
typedef enum {
    HEADER_PACKET               = 0x00,
    SENSOR_DATA_PACKET          = 0x01,
    SENSOR_DESCRIPTION_PACKET   = 0x02,
    HEARTBEAT_PACKET            = 0xFD,
    CONTROLLER_READY_PACKET     = 0xFE,
    TERMINATOR_PACKET           = 0xFF
} PacketIdentifier;

// Type of reading a sensor will produce
typedef enum {
    INT_READING                 = 0x01,         // 16-bit integer
    FLOAT_READING               = 0x02,         // Floating point value
    BOOL_READING                = 0x03          // On/Off value (sent as unsigned 8-bit integer)
} MsgPackReadingType;

typedef enum {
    SONAR_SENSOR_READING_INDEX          = 0,
    MOISTURE_SENSOR_READING_INDEX       = 0,
    DHT22_TEMPERATURE_READING_INDEX     = 0,
    DHT22_HUMIDITY_READING_INDEX        = 1
} ReadingIndex;

// Union containing the actual underlying reading value
typedef union {
    uint16_t mIntValue;
    float mFloatValue;   
    uint8_t mBoolValue; 
} MsgPackReadingValue;


// Description of the sensor reading output (metadata - not an actual reading)
typedef struct {
    const uint8_t mReadingID;               // Reading identifier
    const char *mReadingName;               // Name of reading
    MsgPackReadingType mType;               // The type of reading this sensor provides (i.e. int, float)
    MsgPackReadingValue mMinValue;          // Minimum value in the range of readings for this sensor
    MsgPackReadingValue mMaxValue;          // Maximum value in the range of readings for this sensor
} MsgPackSensorReadingDescription;


// Container for an individual reading value. Also contains a description of the reading
typedef struct {
    const MsgPackSensorReadingDescription* const mDescription;
    MsgPackReadingValue mValue;    
} MsgPackSensorReading;

// Parameters defining how this sensor can be calibrated (if at all)
typedef struct {
    bool mIsCalibratable;                       // Whether this sensor can be calibrated or not
    MsgPackReadingType mCalibrationValueType;   // The type of parameter which will be used to calibrate this sensor
    MsgPackReadingValue mCalibrationRangeMin;   // The minimum of the range the calibration parameter can take
    MsgPackReadingValue mCalibrationRangeMax;   // The maximum of the range the calibration parameter can take
} MsgPackSensorCalibrationParameters;

typedef struct {
    uint8_t mSensorID;
    MsgPackReadingType mCalibrationType;
    MsgPackReadingValue mCalibrationValue;
} MsgPackCalibrationValue;

typedef struct {
    uint8_t mSensorID;                                          // Unique sensor identification value
    const char *mSensorName;                                    // Sensor name/description
    SensorStatus mStatus;                                       // Current status of sensor
    MsgPackSensorCalibrationParameters mCalibrationParams;      // Calibration type
    int mNumReadings;                                           // How many individual reading types this sensor provides (i.e. size of below array)
    MsgPackSensorReading *mSensorReadings;                      // Each individual sensor reading
} MsgPackSensorData;


typedef struct {
    SensorCommandIdentifier mCommandID;     // The command we are responding to
    CommandResponseCode mResponseCode;      // The response code for the issued command
} HeaderPacket;


// msgpack packing status response
typedef struct {        
    size_t mBytesUsed;                      // Number of bytes actually used by packing the data
    int mErrorCode;                         // Any underlying mpack error code returned during packing (0 = no error - see mpack_error_t in mpack.h for more values)
} PackResponse;


MsgPackCalibrationValue unpack_calibration_value(char *input, int inputSize);

// Pack a heartbeat packet
PackResponse pack_heartbeat_packet(char* outBuf, size_t outBufSize);

// Packs a response indicating sensor controller is now ready for comms
PackResponse pack_controller_ready_packet(char* outBuf, size_t outBufSize);

// Packs the header for a response to an issued command
PackResponse pack_header_data(HeaderPacket headerPacket, char* outBuf, size_t outBufSize);

// Packs the packet which signifies and end of response
PackResponse pack_terminator_packet(uint8_t terminatorCode, char* outBuf, size_t outBufSize);

// Packs a data packet for a single sensor
PackResponse pack_sensor_data(const MsgPackSensorData * const sensorData, char* outBuf, size_t outBufSize);

#endif  // SENSOR_MSGPACK_H
