#include "uart_sensor_controller.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sensor_definitions.h"
#include "debug_io.h"
#include "utils.h"


const uint32_t HEARTBEAT_TIMEOUT_MS = 5000;


void reset_controller_interface(ControllerInterface *controllerInterface, bool resetHeartbeat);
void send_heartbeat(ControllerInterface *controllerInterface);
void handle_incoming_byte(ControllerInterface *controllerInterface, uint8_t b);
void handle_sensor_controller_command(
    ControllerInterface *controllerInterface,
    MsgPackSensorPacket *sensorPackets,
    uint8_t numSensors
);
void handle_send_all_sensor_data_command(
    ControllerInterface *controllerInterface,
    MsgPackSensorPacket *sensorPackets,
    uint8_t numSensors
);
void handle_send_sensor_data_command(
    uint8_t sensorID,
    ControllerInterface *controllerInterface,
    MsgPackSensorPacket *sensorPackets,
    uint8_t numSensors
);
void write_msgpack_bytes(
    ControllerInterface *controllerInterface,
    size_t numBytes
) {
    uart_write_blocking(controllerInterface->mUART, controllerInterface->mMsgPackOutputBuffer, numBytes);
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

void handle_calibrate_sensor_command(
    MsgPackSensorPacket *sensorPackets,
    int numSensors, 
    uint8_t *calibrationValueBytes
) {
    // TODO: This whole function needs to be rethunk and rewritten/ditched

    MsgPackCalibrationValue calibration = unpack_calibration_value(calibrationValueBytes, ARGUMENT_LENGTH);

    for(int i = 0; i < numSensors; ++i) {
        if(sensorPackets[i].mSensorID == calibration.mSensorID) {
            break;
        }
    }
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
    MsgPackSensorPacket *sensorPackets,
    uint8_t numSensors
) {
    uint8_t *argumentBytes = &(controllerInterface->mCommandBuffer[1]);

    uint8_t sensorID = 0;

    switch(controllerInterface->mCurrentCommand) {
        case GET_ALL_SENSOR_VALUES:
            handle_send_all_sensor_data_command(
                controllerInterface,
                sensorPackets,
                numSensors
            );
            break;
        case GET_SENSOR_VALUE:
            sensorID = argumentBytes[0];
            // handle_send_sensor_data_command(sensorID);
            break;
        case GET_SENSORS_READY:
            send_controller_ready(controllerInterface);
            break;
        case CALIBRATE_SENSOR:
            handle_calibrate_sensor_command(sensorPackets, numSensors, argumentBytes);
            break;
        case NO_COMMAND:
        default:
            break;
    }
}

// Send sensor data from all sensors
void handle_send_all_sensor_data_command(
    ControllerInterface *controllerInterface,
    MsgPackSensorPacket *sensorPackets,
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
        MsgPackSensorPacket *m = &controllerInterface->mMsgPackSensors[i];

        response = pack_sensor_packet(
            m,
            controllerInterface->mMsgPackOutputBuffer,
            MPACK_OUT_BUFFER_SIZE
        );

        if(!response.mErrorCode) {
            write_msgpack_bytes(controllerInterface, response.mBytesUsed);
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
    MsgPackSensorPacket *sensorPackets,
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
    
    // Pack and send the header data
    response = pack_header_data(headerPacket, controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }

    // Pack and send sensor data
    if(headerPacket.mResponseCode == COMMAND_OK) {
        response = pack_sensor_packet(
            &controllerInterface->mMsgPackSensors[sensorID], 
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
}

// Transmit a single packet signalling the system is ready for data
void send_controller_ready(ControllerInterface *controllerInterface) {
    PackResponse response;

    // Pack and send the header data
    response = pack_controller_ready_packet(controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }

    // Pack and send terminator packet
    response = pack_terminator_packet(CONTROLLER_READY_PACKET,controllerInterface->mMsgPackOutputBuffer, MPACK_OUT_BUFFER_SIZE);
    if(!response.mErrorCode) {
        write_msgpack_bytes(controllerInterface, response.mBytesUsed);
    }
}

// Perform updates - will read from serial interface and if necessary transmit a response. Blocking
bool update_uart_sensor_controller(
    ControllerInterface *controllerInterface
) {
    MsgPackSensorPacket *sensorPackets = controllerInterface->mMsgPackSensors;
    uint8_t numSensors = controllerInterface->mNumMsgPackSensors;

    // Check for heartbeat
    uint32_t currentTimeMS = MILLIS();
    if(currentTimeMS > controllerInterface->mNextHeartbeatTime) {
        send_heartbeat(controllerInterface);
        controllerInterface->mNextHeartbeatTime = (currentTimeMS + HEARTBEAT_TIMEOUT_MS);
    }

    // Ready incoming bytes and handle responses if appropriate
    // Loop until we run out of data or hit a complete command. This stops us just looping forever in this function
    // if the remote end is flooding us with data
    bool loopControl = true;
    while (loopControl) {
        if(uart_is_readable(controllerInterface->mUART)) {
            handle_incoming_byte(controllerInterface, uart_getc(controllerInterface->mUART));
        } else {
            loopControl = false;
        }

        switch(controllerInterface->mCommandBufferState) {
            case AWAITING_DATA:
                break;
            case PROCESSING_COMMAND_DATA:
                break;
            case HAS_COMPLETE_COMMAND:
                handle_sensor_controller_command(
                    controllerInterface,
                    sensorPackets,
                    numSensors
                );
                reset_controller_interface(controllerInterface, false);            
                loopControl = false;
                break;
            case HAS_INVALID_COMMAND_DATA:
                reset_controller_interface(controllerInterface, false);            
                loopControl = false;
                break;
            default:
                break;
        }
    }

    return true;
}
