#ifndef UART_SENSOR_CONTROLLER_H
#define UART_SENSOR_CONTROLLER_H

#include "hardware/uart.h"
#include "hardware/sensors/sensor.h"
#include "command_definitions.h"
#include "sensor_msgpack.h"
#include "pico/util/queue.h"


#define ARGUMENT_LENGTH         (8)
#define COMMAND_LENGTH          (ARGUMENT_LENGTH + 1 + 1)   // Argument bytes +1 byte for command ID and +1 byte for checksum
#define MPACK_OUT_BUFFER_SIZE   (1024)


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
    MsgPackSensorPacket *mMsgPackSensors;                   // Description and data storage objects for outgoing packed data
    uint8_t mNumMsgPackSensors;                             // Number of elements in above array
    queue_t *mSensorUpdateQueue;                            // The inter-core queue for passing sensor data updates between cores
    uint mSerialLEDPin;                                     // Pin for indicating serial communications via an LED
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
bool update_uart_sensor_controller(
    ControllerInterface *controllerInterface
);

// Sends a packet through the serial interface indicating that the controller is ready
void send_controller_ready(ControllerInterface *controllerInterface);

#endif  // SENSOR_CONTROLLER_H
