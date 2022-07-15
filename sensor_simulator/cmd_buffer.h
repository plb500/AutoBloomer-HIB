#ifndef __CMD_BUFFFER_H__
#define __CMD_BUFFFER_H__

#include <Arduino.h>

#define COMMAND_LENGTH (4)


#ifdef __cplusplus
extern "C"{
#endif


typedef enum { 
    COMMAND_START_BYTE          = 0xFF,

    // Commands
    NO_COMMAND                  = 0x00,
    GET_ALL_SENSOR_VALUES       = 0x01,
    GET_SENSOR_VALUE            = 0x02,
    CONTROLLER_PING             = 0x03
} SensorCommandValue;

typedef enum {
    RESPONSE_START_BYTE         = 0xFF,
    SENSOR_DATA_PACKET_START    = 0xFE,

    SENSOR_DATA                 = 0x01,
    SENSOR_DATA_ERROR           = 0x03,
    CONTROLLER_PONG             = 0xEE
} SensorDataResponseValue;

typedef enum {
    NO_SENSOR_DATA              = 0x01,
    VALID_SENSOR_DATA           = 0x02
} SensorDataResponseStatus;

typedef enum {
    ERROR_INVALID_SENSOR_ID     = 0x01
} SensorResponseError;

typedef enum {
    AWAITING_DATA               = 0x00,
    PROCESSING_COMMAND_DATA     = 0x01,
    HAS_COMPLETE_COMMAND        = 0x02,
    HAS_INVALID_COMMAND_DATA    = 0x03
} CommandBufferState;


typedef struct {
    CommandBufferState mCommandBufferState;
    SensorCommandValue mCurrentCommand;
    uint8_t mCommandBuffer[COMMAND_LENGTH];
    uint8_t mCurrentBufferPos;
} CommandBuffer;


// Command buffer functions
void init_cmd_buffer(CommandBuffer *cb);
void handle_incoming_byte(CommandBuffer *cb, uint8_t *in);
void process_cmd_buffer(CommandBuffer *cb);


#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __CMD_BUFFFER_H__
