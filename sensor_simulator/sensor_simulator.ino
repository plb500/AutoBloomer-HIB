#include "cmd_buffer.h"
#include "sensor_msgpack_data.h"
#include "fake_sensors.h"

const size_t MPACK_OUT_BUFFER_SIZE = 512;

CommandBuffer cmdBuffer;
char mpackBuffer[MPACK_OUT_BUFFER_SIZE];


void send_controller_ready() {
    PackResponse response;

    // Pack and send the header data
    response = pack_controller_ready_packet(mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }
}


void handle_send_all_sensor_descriptions_command() {
    PackResponse response;
    HeaderPacket headerPacket = {
        COMMAND_OK,
        NUM_FAKE_SENSORS
    };

    // Pack and send the header data
    response = pack_header_data(headerPacket, mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }

    // Pack and send sensor description
    for(int i = 0; i < NUM_FAKE_SENSORS; ++i) {
        response = pack_sensor_description(FAKE_SENSOR_DESCRIPTIONS[i], mpackBuffer, MPACK_OUT_BUFFER_SIZE);
        if(!response.mErrorCode) {
            Serial.write(mpackBuffer, response.mBytesUsed);
        }
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }
}

void handle_send_sensor_description_command(uint8_t sensorID) {
    PackResponse response;
    HeaderPacket headerPacket;

    // Setup the header
    if(sensorID < NUM_FAKE_SENSORS) {
        headerPacket.mResponseCode = COMMAND_OK;
        headerPacket.mSensorDataCount = 1;        
    } else {
        headerPacket.mResponseCode = SENSOR_NOT_FOUND;
        headerPacket.mSensorDataCount = 0;
    }
    
    // Pack and send the header data
    response = pack_header_data(headerPacket, mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }

    // Pack and send sensor description
    if(sensorID < NUM_FAKE_SENSORS) {
        response = pack_sensor_description(FAKE_SENSOR_DESCRIPTIONS[sensorID], mpackBuffer, MPACK_OUT_BUFFER_SIZE);
        if(!response.mErrorCode) {
            Serial.write(mpackBuffer, response.mBytesUsed);
        }
    }            

    // Pack and send terminator packet
    response = pack_terminator_packet(mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }
}

void handle_send_all_sensor_data_command() {
    PackResponse response;
    HeaderPacket headerPacket = {
        COMMAND_OK,
        NUM_FAKE_SENSORS
    };

    // Pack and send the header data
    response = pack_header_data(headerPacket, mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }

    // Pack and send sensor data
    for(int i = 0; i < NUM_FAKE_SENSORS; ++i) {
        response = pack_sensor_data(FAKE_SENSORS[i], mpackBuffer, MPACK_OUT_BUFFER_SIZE);
        if(!response.mErrorCode) {
            Serial.write(mpackBuffer, response.mBytesUsed);
        }
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }
}

void handle_send_sensor_data_command(uint8_t sensorID) {
    PackResponse response;
    HeaderPacket headerPacket;

    // Setup the header
    if(sensorID < NUM_FAKE_SENSORS) {
        headerPacket.mResponseCode = COMMAND_OK;
        headerPacket.mSensorDataCount = 1;        
    } else {
        headerPacket.mResponseCode = SENSOR_NOT_FOUND;
        headerPacket.mSensorDataCount = 0;
    }
    
    
    // Pack and send the header data
    response = pack_header_data(headerPacket, mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }

    // Pack and send sensor data
    if(sensorID < NUM_FAKE_SENSORS) {
        response = pack_sensor_data(FAKE_SENSORS[sensorID], mpackBuffer, MPACK_OUT_BUFFER_SIZE);
        if(!response.mErrorCode) {
            Serial.write(mpackBuffer, response.mBytesUsed);
        }
    }            

    // Pack and send terminator packet
    response = pack_terminator_packet(mpackBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        Serial.write(mpackBuffer, response.mBytesUsed);
    }
}

void handle_command(SensorCommandIdentifier cmd, uint8_t *argumentBytes) {
    uint8_t sensorID = 0;

    switch(cmd) {
        case GET_ALL_SENSOR_VALUES:
            handle_send_all_sensor_data_command();
            break;
        case GET_SENSOR_VALUE:
            sensorID = argumentBytes[0];
            handle_send_sensor_data_command(sensorID);
            break;
        case GET_ALL_SENSOR_DESCRIPTIONS:
            handle_send_all_sensor_descriptions_command();
            break;
        case GET_SENSOR_DESCRIPTION:
            sensorID = argumentBytes[0];
            handle_send_sensor_description_command(sensorID);
            break;
        case NO_COMMAND:
        default:
            break;
    }
    
    init_cmd_buffer(&cmdBuffer);
}

void setup() {
    // Setup incoming command buffer
    init_cmd_buffer(&cmdBuffer);

    // Setup serial
    Serial.begin(115200);

    // Send ready signal
    send_controller_ready();
}

void loop() {
    uint8_t *argumentBytes;
    
    while(Serial.available()) {
        handle_incoming_byte(&cmdBuffer, Serial.read());
    }

    switch(cmdBuffer.mCommandBufferState) {
        case AWAITING_DATA:
        case PROCESSING_COMMAND_DATA:
            break;
        case HAS_COMPLETE_COMMAND:
            argumentBytes = &(cmdBuffer.mCommandBuffer[1]);
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
