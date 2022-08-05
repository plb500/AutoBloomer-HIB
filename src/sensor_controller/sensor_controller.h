#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include "hardware/uart.h"
#include "hardware/sensors/sensor.h"
#include "command_definitions.h"
#include "hardware/sensors/sensor_msgpack.h"


#define COMMAND_LENGTH (4)
#define MPACK_OUT_BUFFER_SIZE (512)


// States in which the incoming command buffer can be
typedef enum {
    AWAITING_DATA               = 0x00,
    PROCESSING_COMMAND_DATA     = 0x01,
    HAS_COMPLETE_COMMAND        = 0x02,
    HAS_INVALID_COMMAND_DATA    = 0x03
} CommandBufferState;


typedef struct {
    uart_inst_t *mUART;                                     // The UART instance for processing incoming data
    CommandBufferState mCommandBufferState;                 // Current state of the command buffer
    SensorCommandIdentifier mCurrentCommand;                // The current command the command buffer is processing
    uint8_t mCommandBuffer[COMMAND_LENGTH];                 // Buffer for storing incoming serial bytes
    uint8_t mCurrentBufferPos;                              // Current write position in the incoming buffer
    uint8_t mMsgPackOutputBuffer[MPACK_OUT_BUFFER_SIZE];    // Byte buffer for outgoing (mpack) serial data
    uint32_t mNextHeartbeatTime;                            // Time for next heartbeat output pulse
    MsgPackSensorData **mMsgPackSensors;                    // Description and data storage objects for outgoing packed data
    uint8_t mNumMsgPackSensors;                             // Number of elements in above array
} ControllerInterface;


// Initialize the sensor controller interface
void init_sensor_controller(
    ControllerInterface *controllerInterface,
    int txPin,
    int rxPin,
    uint baudrate
);

// Perform an update (read from serial port/push command response data) on the
// controller interface
bool update_sensor_controller(
    ControllerInterface *controllerInterface, 
    SensorData *sensorData,
    uint8_t numSensors
);

// Sends a packet through the serial interface indicating that the controller is ready
void send_controller_ready(ControllerInterface *controllerInterface);

#endif  // SENSOR_CONTROLLER_H
