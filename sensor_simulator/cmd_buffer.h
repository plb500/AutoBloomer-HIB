#ifndef __CMD_BUFFFER_H__
#define __CMD_BUFFFER_H__

#include <Arduino.h>

#define COMMAND_LENGTH (4)


#ifdef __cplusplus
extern "C"{
#endif


typedef enum { 
    NO_COMMAND                  = 0x00,
    GET_ALL_SENSOR_VALUES       = 0x01,
    GET_SENSOR_VALUE            = 0x02,
    GET_ALL_SENSOR_DESCRIPTIONS = 0x03,
    GET_SENSOR_DESCRIPTION      = 0x04
} SensorCommandIdentifier;

typedef enum { 
    COMMAND_START_BYTE          = 0xFF,
} CommandByte;

typedef enum {
    AWAITING_DATA               = 0x00,
    PROCESSING_COMMAND_DATA     = 0x01,
    HAS_COMPLETE_COMMAND        = 0x02,
    HAS_INVALID_COMMAND_DATA    = 0x03
} CommandBufferState;


typedef struct {
    CommandBufferState mCommandBufferState;
    SensorCommandIdentifier mCurrentCommand;
    uint8_t mCommandBuffer[COMMAND_LENGTH];
    uint8_t mCurrentBufferPos;
} CommandBuffer;


// Command buffer functions
void init_cmd_buffer(CommandBuffer *cb);
void handle_incoming_byte(CommandBuffer *cb, char in);
void process_cmd_buffer(CommandBuffer *cb);


#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __CMD_BUFFFER_H__
