#include "cmd_buffer.h"

void init_cmd_buffer(CommandBuffer *cb) {
    cb->mCommandBufferState = AWAITING_DATA;
    cb->mCurrentBufferPos = 0;
    cb->mCurrentCommand = NO_COMMAND;
}

void handle_incoming_byte(CommandBuffer *cb, uint8_t *in) {
    if(in == COMMAND_START_BYTE) {
        init_cmd_buffer(cb);
        return;
    }
    
    cb->mCommandBuffer[cb->mCurrentBufferPos++] = in;

    if (cb->mCurrentBufferPos != COMMAND_LENGTH) {
        cb->mCommandBufferState = PROCESSING_COMMAND_DATA;
        return;
    }
    
    cb->mCurrentBufferPos = 0;

    // Validate checksum
    uint16_t checksum = 0;
    for (int i = 0; i < COMMAND_LENGTH - 1; ++i) {
        checksum += cb->mCommandBuffer[i];
    }
    if ((checksum & 0xFF) !=
        cb->mCommandBuffer[COMMAND_LENGTH - 1]) {
        cb->mCommandBufferState = HAS_INVALID_COMMAND_DATA;
        return;
    }

    // Complete, valid command. Process
    cb->mCommandBufferState = HAS_COMPLETE_COMMAND;
    cb->mCurrentCommand = (SensorCommandValue) cb->mCommandBuffer[0];
}
