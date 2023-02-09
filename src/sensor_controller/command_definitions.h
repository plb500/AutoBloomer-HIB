#ifndef COMMAND_DEFINITIONS_H
#define COMMAND_DEFINITIONS_H

// Sensor commands
typedef enum { 
    NO_COMMAND                  = 0x00,
    GET_ALL_SENSOR_VALUES       = 0x01,
    GET_SENSOR_VALUE            = 0x02,
    GET_SENSORS_READY           = 0x03,
    CALIBRATE_SENSOR            = 0x04
} SensorCommandIdentifier;


// Command response codes 
typedef enum {
    COMMAND_OK                  = 0x00,
    SENSOR_NOT_FOUND            = 0x01,
    HEARTBEAT                   = 0xFE,
    CONTROLLER_READY            = 0xFF
} CommandResponseCode;


// Incoming byte value definitions
typedef enum { 
    COMMAND_START_BYTE          = 0xFF,
} CommandByte;

#endif
