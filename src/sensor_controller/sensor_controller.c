#include "sensor_controller.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/gpio.h"

// We can't use 0xFF or 0xFE so the max value for any single byte we can possibly have is 0xFD
#define MAX_BYTE_VALUE                          (0xFD)
#define MAX_UINT16_VALUE                        (0xFDFD)
#define PACK_BYTE(value)                        (MIN(MAX_BYTE_VALUE, value))
#define PACK_UINT16(value)                      (MIN(MAX_UINT16_VALUE, value))
#define PACK_FLOAT(value, maxValue)             (PACK_UINT16((uint16_t)round((value / maxValue) * MAX_UINT16_VALUE)))

// Sensor Data Packet Positions
#define SENSOR_DATA_PACKET_START_POS            (0)
#define SENSOR_DATA_PACKET_SIZE_POS             (1)
#define SENSOR_DATA_PACKET_ID_POS               (2)
#define SENSOR_DATA_PACKET_STATUS_POS           (3)
#define SENSOR_DATA_PACKET_READING_POS          (4)

#define SENSOR_DATA_RESPONSE_START_POS          (0)
#define SENSOR_DATA_RESPONSE_SIZE_POS           (1)
#define SENSOR_DATA_RESPONSE_TYPE_POS           (2)
#define SENSOR_DATA_RESPONSE_DATA_POS           (3)

#define SENSOR_DATA_ERROR_TYPE_POS              (3)
#define SENSOR_DATA_ERROR_CHECKSUM_POS          (4)


void handle_sensor_controller_command(ControllerInterface *controllerInterface,
                                      SensorCommandValue cmd,
                                      uint8_t *argumentBytes,
                                      SensorData *sensorData,
                                      uint8_t numSensors);

#define OUTGOING_BYTE_BUFFER_SIZE               (128)
uint8_t _outgoingBytesBuffer[OUTGOING_BYTE_BUFFER_SIZE];

uint8_t pack_sensor_data_packet(SensorData *sensorData, uint8_t sensorID, uint8_t *buffer) {
    buffer[SENSOR_DATA_PACKET_START_POS] = SENSOR_DATA_PACKET_START;
    buffer[SENSOR_DATA_PACKET_ID_POS] = sensorID;
    buffer[SENSOR_DATA_PACKET_SIZE_POS] = 1;        // We've only added the ID to the data packet so far

    if(sensorData->mIsSensorReadingValid) {
        switch(sensorData->mType) {
            case LOAD_SENSOR_DATA:
            {
                // Add sensor status
                buffer[SENSOR_DATA_PACKET_STATUS_POS] = VALID_SENSOR_DATA;
                buffer[SENSOR_DATA_PACKET_SIZE_POS] += 1;       // Data packet size: 2

                // Add sensor reading
                uint16_t packedData = PACK_FLOAT(sensorData->mSensorReading.mLoadSensorWeight, LOAD_SENSOR_MAX_VALUE);
                buffer[SENSOR_DATA_PACKET_READING_POS] = ((packedData & 0xFF00) >> 8);
                buffer[SENSOR_DATA_PACKET_READING_POS + 1] = (packedData & 0x00FF);
                buffer[SENSOR_DATA_PACKET_SIZE_POS] += 2;       // Data packet size: 4
                break;
            }
            case MOISTURE_SENSOR_DATA:
            {
                // Add sensor status
                buffer[SENSOR_DATA_PACKET_STATUS_POS] = VALID_SENSOR_DATA;
                buffer[SENSOR_DATA_PACKET_SIZE_POS] += 1;       // Data packet size: 2

                // Add sensor reading
                uint16_t cappedData = PACK_UINT16(sensorData->mSensorReading.mMoistureSensorValue);
                buffer[SENSOR_DATA_PACKET_READING_POS] = ((cappedData & 0xFF00) >> 8);
                buffer[SENSOR_DATA_PACKET_READING_POS + 1] = (cappedData & 0x00FF);
                buffer[SENSOR_DATA_PACKET_SIZE_POS] += 2;       // Data packet size: 4
                break;
            }
            case TEMP_HUMIDITY_DATA:
                if(sensorData->mSensorReading.mTempHumidityData.mReadingError == NO_ERROR) {
                    // Add sensor status
                    buffer[SENSOR_DATA_PACKET_STATUS_POS] = VALID_SENSOR_DATA;
                    buffer[SENSOR_DATA_PACKET_SIZE_POS] += 1;       // Data packet size: 2

                    // Add sensor readings
                    uint16_t packedTempData = PACK_FLOAT(sensorData->mSensorReading.mTempHumidityData.mTemperatureC, TEMP_SENSOR_MAX_VALUE);
                    buffer[SENSOR_DATA_PACKET_READING_POS] = ((packedTempData & 0xFF00) >> 8);
                    buffer[SENSOR_DATA_PACKET_READING_POS + 1] = (packedTempData & 0x00FF);
                    buffer[SENSOR_DATA_PACKET_SIZE_POS] += 2;       // Data packet size: 4

                    uint16_t packedHumidityData = PACK_FLOAT(sensorData->mSensorReading.mTempHumidityData.mRelativeHumidity, RH_SENSOR_MAX_VALUE);
                    buffer[SENSOR_DATA_PACKET_READING_POS + 2] = ((packedHumidityData & 0xFF00) >> 8);
                    buffer[SENSOR_DATA_PACKET_READING_POS + 3] = (packedHumidityData & 0x00FF);
                    buffer[SENSOR_DATA_PACKET_SIZE_POS] += 4;       // Data packet size: 6
                } else {
                    // Add sensor status
                    buffer[SENSOR_DATA_PACKET_STATUS_POS] = SENSOR_DATA_ERROR;
                    buffer[SENSOR_DATA_PACKET_SIZE_POS] += 1;       // Data packet size: 2
                }
                break;
            default:
                break;
        }
    } else {
        buffer[SENSOR_DATA_PACKET_STATUS_POS] = NO_SENSOR_DATA;
        buffer[SENSOR_DATA_PACKET_SIZE_POS] += 1;       // Data packet size: 2
    }

    return (buffer[SENSOR_DATA_PACKET_SIZE_POS] + 2);
}

uint8_t pack_sensor_pong_response(uint8_t *buffer) {
    buffer[SENSOR_DATA_RESPONSE_START_POS] = RESPONSE_START_BYTE;
    buffer[SENSOR_DATA_RESPONSE_TYPE_POS] = CONTROLLER_PONG;
    buffer[SENSOR_DATA_RESPONSE_SIZE_POS] = 1;    // We've only added the response type to the data packet so far

    return 3;
}

uint8_t pack_sensor_data_response(SensorData *sensorData, uint8_t numSensors, uint8_t *buffer) {
    buffer[SENSOR_DATA_RESPONSE_START_POS] = RESPONSE_START_BYTE;
    buffer[SENSOR_DATA_RESPONSE_TYPE_POS] = SENSOR_DATA;
    buffer[SENSOR_DATA_RESPONSE_SIZE_POS] = 1;    // We've only added the response type to the data packet so far

    // Pack in sensor data
    uint8_t *dst = &buffer[SENSOR_DATA_RESPONSE_DATA_POS];
    uint16_t sensorDataCount = 0;
    for(int s = 0; s < numSensors; ++s) {
        sensorDataCount += pack_sensor_data_packet(&sensorData[s], sensorData[s].mSensorID, dst);
        dst = &buffer[SENSOR_DATA_RESPONSE_DATA_POS + sensorDataCount];
    }

    buffer[SENSOR_DATA_RESPONSE_SIZE_POS] += sensorDataCount;

    // Calculate and insert checksum into buffer
    uint16_t checksum = 0;
    for(int i = SENSOR_DATA_RESPONSE_TYPE_POS; i <= buffer[SENSOR_DATA_RESPONSE_SIZE_POS]; ++i) {
        checksum += buffer[i];
    }
    buffer[SENSOR_DATA_RESPONSE_DATA_POS + sensorDataCount] = (uint8_t)(checksum & MAX_BYTE_VALUE);
    buffer[SENSOR_DATA_RESPONSE_SIZE_POS] += 1;

    return (buffer[SENSOR_DATA_RESPONSE_SIZE_POS] + 2);
}

uint8_t pack_sensor_error_response(SensorResponseError error, uint8_t *buffer) {
    buffer[SENSOR_DATA_RESPONSE_START_POS] = RESPONSE_START_BYTE;
    buffer[SENSOR_DATA_RESPONSE_SIZE_POS] = 3;
    buffer[SENSOR_DATA_RESPONSE_TYPE_POS] = SENSOR_DATA_ERROR;
    buffer[SENSOR_DATA_ERROR_TYPE_POS] = (uint8_t) error;
    uint16_t checksum = (buffer[SENSOR_DATA_RESPONSE_TYPE_POS] + buffer[SENSOR_DATA_ERROR_TYPE_POS]);
    buffer[SENSOR_DATA_ERROR_CHECKSUM_POS] = (uint8_t) (checksum & 0x00FF);

    return 5;
}


void init_sensor_controller(ControllerInterface *controllerInterface, int txPin,
                            int rxPin, uint baudrate) {
    // Set up our UART with the required speed.
    uart_init(controllerInterface->mUART, baudrate);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(txPin, GPIO_FUNC_UART);
    gpio_set_function(rxPin, GPIO_FUNC_UART);

    controllerInterface->mCurrentBufferPos = 0;

    DEBUG_PRINT("Sensor Controller Ready!\n");
}

bool process_sensor_controller_input(ControllerInterface *controllerInterface,
                                     SensorData *sensorData,
                                     uint8_t numSensors) {
    while (uart_is_readable(controllerInterface->mUART)) {
        uint8_t ch = uart_getc(controllerInterface->mUART);

        if (ch == COMMAND_START_BYTE) {
            controllerInterface->mCurrentBufferPos = 0;
            DEBUG_PRINT("New command!\n");
        } else {
            controllerInterface
                ->mCommandBuffer[controllerInterface->mCurrentBufferPos++] = ch;
            process_sensor_controller_command_data(controllerInterface,
                                                   sensorData, numSensors);
        }
    }

    return true;
}

void process_sensor_controller_command_data(ControllerInterface *controllerInterface, 
                                            SensorData *sensorData,
                                            uint8_t numSensors) {
    if (controllerInterface->mCurrentBufferPos != COMMAND_LENGTH) {
        return;
    }

    controllerInterface->mCurrentBufferPos = 0;

    // There is a complete command
    DEBUG_PRINT("Command bytes received\n");

    // Validate checksum
    uint16_t checksum = 0;
    for (int i = 0; i < COMMAND_LENGTH - 1; ++i) {
        checksum += controllerInterface->mCommandBuffer[i];
    }
    if ((checksum & 0xFF) !=
        controllerInterface->mCommandBuffer[COMMAND_LENGTH - 1]) {
        DEBUG_PRINT(" * Command checksum validation failed!\n");
        return;
    }

    // Complete, valid command. Process
    DEBUG_PRINT(" * Checksum validation success!\n");
    SensorCommandValue cmd =
        (SensorCommandValue)controllerInterface->mCommandBuffer[0];
    uint8_t *argumentBytes = &(controllerInterface->mCommandBuffer[1]);
    handle_sensor_controller_command(controllerInterface, cmd, argumentBytes, sensorData, numSensors);
}

void handle_sensor_controller_command(ControllerInterface *controllerInterface,
                                      SensorCommandValue cmd,
                                      uint8_t *argumentBytes,
                                      SensorData *sensorData,
                                      uint8_t numSensors) {
    uint8_t responseLen = 0;

    switch (cmd) {
        case GET_ALL_SENSOR_VALUES:
            DEBUG_PRINT("Command: GET_ALL_SENSOR_VALUES\n");
            responseLen = pack_sensor_data_response(sensorData, numSensors, _outgoingBytesBuffer);
            break;

        case GET_SENSOR_VALUE: {
            DEBUG_PRINT("Command: GET_SENSOR_VALUE\n");
            uint8_t sensorID = *argumentBytes;
            if (sensorID < numSensors) {
                DEBUG_PRINT(" * Sensor %d is valid\n", sensorID);
                responseLen = pack_sensor_data_response(&sensorData[sensorID], 1, _outgoingBytesBuffer);
            } else {
                responseLen = pack_sensor_error_response(ERROR_INVALID_SENSOR_ID, _outgoingBytesBuffer);
            }
            break;
        }

        case CONTROLLER_PING:
            DEBUG_PRINT("Command: SENSOR PING\n");
            responseLen = pack_sensor_pong_response(_outgoingBytesBuffer);
        
        default:
            DEBUG_PRINT(" * Unknown/Unhandled command: %d\n", cmd);
            break;
    }

    if(responseLen) {
        uart_write_blocking(controllerInterface->mUART, _outgoingBytesBuffer, responseLen);
    }
}
