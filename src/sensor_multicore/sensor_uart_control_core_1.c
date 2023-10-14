#include "sensor_uart_control_core_1.h"
#include "sensor_definitions.h"
#include "sensor_multicore_utils.h"
#include "uart_controller/uart_sensor_controller.h"
#include "debug_io.h"

#include "pico/util/queue.h"

extern ControllerInterface _sensorControllerInterface;

void sensor_controller_core_main() {
    // Transmit "ready" message on core startup
    send_controller_ready(&_sensorControllerInterface);

    // Main execution loop
    while(1) {
        // First thing to do is process any inter-core messages from the sensor update core
        consume_update_queue_messages(
            _sensorControllerInterface.mSensorUpdateQueue,
            _sensorControllerInterface.mMsgPackSensors
        );

        // Secondly, handle any incoming controller commands
        update_uart_sensor_controller(&_sensorControllerInterface);
    }
}
