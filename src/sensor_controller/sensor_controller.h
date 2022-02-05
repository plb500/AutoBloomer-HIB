#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include "hardware/uart.h"
#include "hardware/sensors/sensor.h"

typedef enum { 
    COMMAND_START_BYTE          = 0xFF,

    // Commands
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

                                                // OUTPUT //
                                                // ------ //
// Sensor Data Packet Structure
// [0]   SENSOR_DATA_PACKET_START   - 1 byte
// [1]   Data packet size           - 1 byte
// [2]   Sensor ID                  - 1 byte
// [3]   Sensor Status              - 1 byte
// [4-7] Sensor Reading (if valid)  - 0/2/4 bytes
//                      Total bytes - 4, 6 or 8 bytes

// Single Sensor Data Response Structure:
// [0] RESPONSE_START_BYTE                                          - 1 byte
// [1] Response Size (excluding start marker)                       - 1 byte
// [2] Response Type (SENSOR_DATA)                                  - 1 byte
// [3] Sensor Data Packets                                          - (4, 6 or 8 bytes) * # sensors
// [6 / 8 / 10] Checksum ((response[2] + ... response[z]) & 0xFF)   - 1 byte
//                                                      Total bytes - 4 + ([8|10|12] * # sensors) bytes

// Sensor Data Error Response Structure
// [0] RESPONSE_START_BYTE                                          - 1 byte
// [1] Response Size (excluding start marker)                       - 1 byte
// [2] Response Type (SENSOR_DATA_ERROR)                            - 1 byte
// [3] Error Type (SensorResponseError)                             - 1 byte
// [4] Checksum ((response[2] + response[3]) & 0xFF)                - 1 byte
//                                                      Total bytes - 5 bytes


                                                // INPUT //
                                                // ----- //
// Incoming Command Structure:
// [-]    Command Start Marker (0xFF)                               - 1 byte
// [0]    Command Identifier                                        - 1 byte
// [1, 2] Command Arguments                                         - 2 bytes
// [3]    Checksum  (([0] + [1] + [2] + [3]) & 0xFF)                - 1 byte
#define COMMAND_LENGTH (4)


typedef struct {
    uart_inst_t *mUART;
    uint8_t mCommandBuffer[COMMAND_LENGTH];
    uint8_t mCurrentBufferPos;
} ControllerInterface;


void init_sensor_controller(ControllerInterface *controllerInterface,
                            int txPin,
                            int rxPin,
                            uint baudrate);
bool process_sensor_controller_input(ControllerInterface *controllerInterface, 
                                     SensorData *sensorData,
                                     uint8_t numSensors);
void process_sensor_controller_command_data(ControllerInterface *controllerInterface, 
                                            SensorData *sensorData,
                                            uint8_t numSensors);

#endif  // SENSOR_CONTROLLER_H
