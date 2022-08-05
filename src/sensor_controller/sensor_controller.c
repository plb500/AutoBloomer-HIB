#include "sensor_controller.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sensor_definitions.h"

const uint32_t HEARTBEAT_TIMEOUT_MS = 5000;


uint32_t get_current_time_ms();
void reset_controller_interface(ControllerInterface *controllerInterface, bool resetHeartbeat);
void send_heartbeat(ControllerInterface *controllerInterface);
void handle_incoming_byte(ControllerInterface *controllerInterface, uint8_t b);
bool convert_sensor_data(
    SensorData *sensorData,
    uint8_t numSensors,
    MsgPackSensorData *msgPackSensorData
);
void handle_sensor_controller_command(
    ControllerInterface *controllerInterface,
    SensorData *sensorData,
    uint8_t numSensors
);
void handle_send_all_sensor_data_command(
    ControllerInterface *controllerInterface,
    SensorData *sensorData,
    uint8_t numSensors
);
void handle_send_sensor_data_command(
    uint8_t sensorID,
    ControllerInterface *controllerInterface,
    SensorData *sensorData,
    uint8_t numSensors
);
void write_msgpack_bytes(
    ControllerInterface *controllerInterface,
    size_t numBytes
) {
    uart_write_blocking(controllerInterface->mUART, controllerInterface->mMsgPackOutputBuffer, numBytes);
    uart_tx_wait_blocking(controllerInterface->mUART);
}


// Returns the current time since controller boot
uint32_t get_current_time_ms() {
    return to_ms_since_boot(get_absolute_time());
}

// Resets the interface back to an initial state
void reset_controller_interface(
    ControllerInterface *controllerInterface,
    bool resetHeartbeat
) {
    controllerInterface->mCommandBufferState    = AWAITING_DATA;
    controllerInterface->mCurrentBufferPos      = 0;
    controllerInterface->mCurrentCommand        = NO_COMMAND;

    if(resetHeartbeat) {
        controllerInterface->mNextHeartbeatTime     = 0;
    }
}

// Copy data from active sensor structures into outgoing serial message object
bool convert_sensor_data(
    SensorData *sensorData,
    uint8_t numSensors,
    MsgPackSensorData *msgPackSensorData
) {
    if(!sensorData || !msgPackSensorData) {
        return false;
    }

    // Find the matching sensor data
    SensorData *foundData = 0;
    for(int i = 0; i < numSensors; ++i) {
        if(sensorData[i].mSensorID == msgPackSensorData->mSensorID) {
            foundData = &(sensorData[i]);
            break;
        }
    }

    if(!foundData) {
        return false;
    }

    // First, set the status
    msgPackSensorData->mStatus = CONNECTED_WORKING;
    if(!foundData->mIsSensorConnected) {
        msgPackSensorData->mStatus = DISCONNECTED;
    } else if(!foundData->mIsSensorReadingValid) {
        msgPackSensorData->mStatus = CONNECTED_NOT_WORKING;
    }

    // Next set the actual readings
    switch(foundData->mType) {
        case LOAD_SENSOR:
            msgPackSensorData->mSensorReadings[LOAD_SENSOR_READING_INDEX].mValue.mFloatValue = foundData->mSensorReading.mLoadSensorWeight;
            break;

        case MOISTURE_SENSOR:
            msgPackSensorData->mSensorReadings[MOISTURE_SENSOR_READING_INDEX].mValue.mIntValue = foundData->mSensorReading.mMoistureSensorValue;
            break;

        case TEMP_HUMIDITY_SENSOR:
            msgPackSensorData->mSensorReadings[DHT22_TEMPERATURE_READING_INDEX].mValue.mFloatValue = foundData->mSensorReading.mTempHumidityData.mTemperatureC;
            msgPackSensorData->mSensorReadings[DHT22_HUMIDITY_READING_INDEX].mValue.mFloatValue = foundData->mSensorReading.mTempHumidityData.mRelativeHumidity;
            break;
    }

    return true;
}

// Process an incoming serial byte and create a response, if necessary
void handle_incoming_byte(ControllerInterface *controllerInterface, uint8_t b) {
    if(b == COMMAND_START_BYTE) {
        reset_controller_interface(controllerInterface, false);
        return;
    }
    
    controllerInterface->mCommandBuffer[controllerInterface->mCurrentBufferPos++] = b;

    if(controllerInterface->mCurrentBufferPos != COMMAND_LENGTH) {
        controllerInterface->mCommandBufferState = PROCESSING_COMMAND_DATA;
        return;
    }
    
    // We have a complete command, reset the buffer position and process the command
    controllerInterface->mCurrentBufferPos = 0;

    // Validate checksum
    uint16_t checksum = 0;
    for (int i = 0; i < COMMAND_LENGTH - 1; ++i) {
        checksum += controllerInterface->mCommandBuffer[i];
    }

    // Checksum was invalid, set invalid state and return
    if((checksum & 0xFF) != controllerInterface->mCommandBuffer[COMMAND_LENGTH - 1]) {
        controllerInterface->mCommandBufferState = HAS_INVALID_COMMAND_DATA;
        return;
    }

    // Complete, valid command. Process
    controllerInterface->mCommandBufferState = HAS_COMPLETE_COMMAND;
    controllerInterface->mCurrentCommand = (SensorCommandIdentifier) controllerInterface->mCommandBuffer[0];
}

// Transmit a heartbeat pulse packet
void send_heartbeat(ControllerInterface *controllerInterface) {
    PackResponse response;

    // Pack and send the header data
    response = pack_heartbeat_packet(controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(HEARTBEAT_PACKET, controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }
}

// Process a complete command received via serial interface
void handle_sensor_controller_command(
    ControllerInterface *controllerInterface,
    SensorData *sensorData,
    uint8_t numSensors
) {
    uint8_t *argumentBytes = &(controllerInterface->mCommandBuffer[1]);

    uint8_t sensorID = 0;

    switch(controllerInterface->mCurrentCommand) {
        case GET_ALL_SENSOR_VALUES:
            handle_send_all_sensor_data_command(
                controllerInterface,
                sensorData,
                numSensors
            );
            break;
        case GET_SENSOR_VALUE:
            sensorID = argumentBytes[0];
            // handle_send_sensor_data_command(sensorID);
            break;
        case NO_COMMAND:
        default:
            break;
    }
}

// Send sensor data from all sensors
void handle_send_all_sensor_data_command(
    ControllerInterface *controllerInterface,
    SensorData *sensorData,
    uint8_t numSensors
) {
    PackResponse response;
    HeaderPacket headerPacket = {
        GET_ALL_SENSOR_VALUES,
        COMMAND_OK,
    };

    memset(controllerInterface->mMsgPackOutputBuffer, 0, MPACK_OUT_BUFFER_SIZE);


    // Pack and send the header data
    response = pack_header_data(headerPacket, controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }

    // Pack and send sensor data
    for(int i = 0; i < NUM_SENSORS; ++i) {
        MsgPackSensorData *m = controllerInterface->mMsgPackSensors[i];

        // Convert raw sensor data to msgpack structure
        if(convert_sensor_data(sensorData, numSensors, m)) {
        // if(true) { 
            response = pack_sensor_data(
                m,
                controllerInterface->mMsgPackOutputBuffer,
                MPACK_OUT_BUFFER_SIZE
            );

            if(!response.mErrorCode) {
                write_msgpack_bytes(controllerInterface, response.mBytesUsed);
            }
        }
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(GET_ALL_SENSOR_VALUES, controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }
}

// Send a single piece of sensor data back
void handle_send_sensor_data_command(
    uint8_t sensorID,
    ControllerInterface *controllerInterface,
    SensorData *sensorData,
    uint8_t numSensors
) {
    PackResponse response;
    HeaderPacket headerPacket = {
        GET_SENSOR_VALUE,
        COMMAND_OK    
    };

    // Check for bad sensor ID (should probably check the actual IDs but for now this works)
    if(sensorID >= numSensors) {
        headerPacket.mResponseCode = SENSOR_NOT_FOUND;
    }
    
    // Convert the sensor data for transmission
    if(!convert_sensor_data(
        sensorData,
        numSensors,
        controllerInterface->mMsgPackSensors[sensorID]
    )) {
        headerPacket.mResponseCode = SENSOR_NOT_FOUND;
    }

    // Pack and send the header data
    response = pack_header_data(headerPacket, controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }

    // Pack and send sensor data
    if(headerPacket.mResponseCode == COMMAND_OK) {
        response = pack_sensor_data(
            controllerInterface->mMsgPackSensors[sensorID], 
            controllerInterface->mMsgPackOutputBuffer, 
            MPACK_OUT_BUFFER_SIZE
        );

        if(!response.mErrorCode) {
            write_msgpack_bytes(controllerInterface, response.mBytesUsed);
        }
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(GET_SENSOR_VALUE, controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }
}

// Initialize serial interface and controller port
void init_sensor_controller(
    ControllerInterface *controllerInterface,
    int txPin,
    int rxPin,
    uint baudrate
) {
    // Set up our UART with the required speed.
    uart_init(controllerInterface->mUART, baudrate);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(txPin, GPIO_FUNC_UART);
    gpio_set_function(rxPin, GPIO_FUNC_UART);

    reset_controller_interface(controllerInterface, true);

    DEBUG_PRINT("Sensor Controller Ready!\n");
}

// Transmit a single packet signalling the system is ready for data
void send_controller_ready(ControllerInterface *controllerInterface) {
    PackResponse response;

    // Pack and send the header data
    response = pack_controller_ready_packet(controllerInterface->mMsgPackOutputBuffer, response.mBytesUsed);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(CONTROLLER_READY_PACKET,controllerInterface->mMsgPackOutputBuffer, response.mBytesUsed);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }
}

// Perform updates - will read from serial interface and if necessary transmit a response. Blocking
bool update_sensor_controller(
    ControllerInterface *controllerInterface,
    SensorData *sensorData,
    uint8_t numSensors
) {
    // Check for heartbeat
    uint32_t currentTimeMS = get_current_time_ms();
    if(currentTimeMS > controllerInterface->mNextHeartbeatTime) {
        send_heartbeat(controllerInterface);
        controllerInterface->mNextHeartbeatTime = (currentTimeMS + HEARTBEAT_TIMEOUT_MS);
    }

    // Ready incoming bytes and handle responses if appropriate
    while (uart_is_readable(controllerInterface->mUART)) {
        handle_incoming_byte(controllerInterface, uart_getc(controllerInterface->mUART));

        switch(controllerInterface->mCommandBufferState) {
            case AWAITING_DATA:
            case PROCESSING_COMMAND_DATA:
                break;
            case HAS_COMPLETE_COMMAND:
                handle_sensor_controller_command(
                    controllerInterface,
                    sensorData,
                    numSensors
                );
                reset_controller_interface(controllerInterface, false);            
                break;
            case HAS_INVALID_COMMAND_DATA:
                reset_controller_interface(controllerInterface, false);            
                break;
            default:
                break;
        }
    }

    return true;
}
