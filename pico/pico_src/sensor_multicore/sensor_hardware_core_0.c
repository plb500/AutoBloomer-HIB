#include "hardware/sensors/sensor.h"

#include <string.h>

#include "hardware/watchdog.h"
#include "hardware/flash.h"
#include "hardware/shift_register.h"
#include "hardware/sensors/sensor_i2c_interface.h"
#include "hardware/connected_hardware_monitor.h"

#include "uart_controller/uart_sensor_controller.h"
#include "sensor_uart_control_core_1.h"
#include "sensor_multicore/sensor_multicore_utils.h"
#include "sensor_definitions.h"
#include "debug_io.h"
#include "utils.h"

#include "pico/multicore.h"
#include "pico/util/queue.h"


const uint8_t ONBOARD_LED_PIN = 25;
const bool DEBUG_SENSOR_UPDATE = false;

// Queue used for sending sensor updates from core0 to core1
queue_t sensorUpdateQueue;

// Controller interface for comms running on core 1
ControllerInterface _sensorControllerInterface = {
    .mUART = SENSOR_CONTROLLER_UART,
    .mMsgPackSensors = sensorPackets,
    .mNumMsgPackSensors = NUM_SENSORS,
    .mSensorUpdateQueue = &sensorUpdateQueue,
    .mSerialLEDPin = ONBOARD_LED_PIN
};

// Connection LED controller
ShiftRegister _ledShifter = {
    .mDataPin = SIPO_DATA_PIN,
    .mLatchPin = SIPO_LATCH_PIN,
    .mClockPin = SIPO_CLOCK_PIN,
    .mType = SIPO_SHIFT_REGISTER,
    .mNumBits = 8
};

ConnectedHardwareMonitor _connectedHardwareMonitor = {
    .mConnectedHardwareRegister = {
        .mDataPin = HARDWARE_CONNECT_SR_DATA_PIN,
        .mLatchPin = HARDWARE_CONNECT_SR_LATCH_PIN,
        .mClockPin = HARDWARE_CONNECT_SR_CLOCK_PIN,
        .mType = PISO_SHIFT_REGISTER,
        .mNumBits = 16
    }
};


void update_sensor_status_indicators(ShiftRegister *shiftRegister, Sensor* sensors, uint8_t numSensors) {
    for(int i = 0; i < numSensors; ++i) {
        if(sensors[i].mSensorDefinition.mSensorConnectLEDPosition != NO_LED) {
            bool ledOn = (sensors[i].mCurrentSensorData.mSensorStatus == SENSOR_CONNECTED_VALID_DATA);
            set_shift_register_state(shiftRegister, sensors[i].mSensorDefinition.mSensorConnectLEDPosition, ledOn);
        }
    }
    write_shift_register_states(shiftRegister);
}

int main() {
    // Initialize debug serial output
    DEBUG_PRINT_INIT();

    DEBUG_PRINT("Sensor hardware core start\n");
    if (watchdog_caused_reboot()) {
        DEBUG_PRINT("Rebooted by Watchdog!\n");
    } else {
        DEBUG_PRINT("Clean boot\n");
    }

    // Enable the watchdog, requiring the watchdog to be updated every 5000ms or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    watchdog_enable(5000, 1);

    // Initialize onboard LED
    gpio_init(ONBOARD_LED_PIN);
    gpio_set_dir(ONBOARD_LED_PIN, GPIO_OUT);
    gpio_put(ONBOARD_LED_PIN, false);

    DEBUG_PRINT("LEDs initialized\n");

    // Initialize the I2C interface
    init_sensor_bus(&sensorI2CInterface);

    DEBUG_PRINT("Sensor bus initialized\n");

    // Initialize status LEDs
    init_shift_register(&_ledShifter);

    // Initialize hardware connection monitor
    init_connected_hardware_monitor(&_connectedHardwareMonitor);

    // Initialize cross-core queue
    intitialize_sensor_data_queue(&sensorUpdateQueue,(NUM_SENSORS * 4));

    DEBUG_PRINT("Sensor data queue ready\n");

    // Initialise UART controller comms interface
    init_sensor_controller(&_sensorControllerInterface, SENSOR_CONTROLLER_TX, SENSOR_CONTROLLER_RX, SENSOR_CONTROLLER_BAUDRATE);

    DEBUG_PRINT("Sensor controller ready, launching controller core\n");

    // Launch secondary core (core 1)
    multicore_launch_core1(sensor_controller_core_main);

    // Activate status indicator
    gpio_put(ONBOARD_LED_PIN, true);

    // core0 execution loop
    DEBUG_PRINT("Sensor initialization complete\n");
    while(1) {
        update_connected_hardware_monitor(&_connectedHardwareMonitor);

        // Update sensor readings
        DEBUG_PRINT("Update sensors\n");
        gpio_put(ONBOARD_LED_PIN, false);
        update_sensors(sensorsList, NUM_SENSORS, DEBUG_SENSOR_UPDATE, &_connectedHardwareMonitor);
        gpio_put(ONBOARD_LED_PIN, true);

        // Update sensor LED indicators
        DEBUG_PRINT("Update LEDs\n");
        update_sensor_status_indicators(&_ledShifter, sensorsList, NUM_SENSORS);

        // Push sensor updates to core 1
        push_sensor_data_to_queue(&sensorUpdateQueue, sensorsList);

        // Pet the watchdog
        watchdog_update();

        // Chill
        sleep_ms(1);
    }
}
