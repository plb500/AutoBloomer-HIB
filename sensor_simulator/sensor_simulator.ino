#include "cmd_buffer.h"
#include "sensor_msgpack_data.h"
#include "fake_sensors.h"

const size_t MPACK_OUT_BUFFER_SIZE = 256;

CommandBuffer cmdBuffer;
uint8_t mpackBuffer[MPACK_OUT_BUFFER_SIZE];


void handle_command(SensorCommandValue cmd, uint8_t *argumentBytes) {
    PackResponse response;
    
    switch(cmd) {
        case GET_ALL_SENSOR_VALUES:
            for(int i = 0; i < NUM_FAKE_SENSORS; ++i) {
                response = pack_sensor_data(FAKE_SENSORS[i], mpackBuffer, MPACK_OUT_BUFFER_SIZE);
                if(!response.mErrorCode) {
                    Serial.write(mpackBuffer, response.mBytesUsed);
                }
            }
            break;
        case GET_SENSOR_VALUE:
            uint8_t sensorID = argumentBytes[0];
            if(sensorID < NUM_FAKE_SENSORS) {
                response = pack_sensor_data(FAKE_SENSORS[sensorID], mpackBuffer, MPACK_OUT_BUFFER_SIZE);
                if(!response.mErrorCode) {
                    Serial.write(mpackBuffer, response.mBytesUsed);
                }
            }            
    }
    
    init_cmd_buffer(&cmdBuffer);
}

void setup() {
    // Setup inoming command buffer
    init_cmd_buffer(&cmdBuffer);

    // Setup serial
    Serial.begin(115200);
}

void loop() {
    uint8_t inByte;
    
    while(Serial.available()) {
        handle_incoming_byte(&cmdBuffer, Serial.read());
    }

    switch(cmdBuffer.mCommandBufferState) {
        case AWAITING_DATA:
        case PROCESSING_COMMAND_DATA:
            break;
        case HAS_COMPLETE_COMMAND:
            uint8_t *argumentBytes = &(cmdBuffer.mCommandBuffer[1]);
            handle_command(cmdBuffer.mCurrentCommand, argumentBytes);
            init_cmd_buffer(&cmdBuffer);            
            break;
        case HAS_INVALID_COMMAND_DATA:
            init_cmd_buffer(&cmdBuffer);
            break;
        default:
            break;
    }
}
