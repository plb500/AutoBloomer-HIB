#include "hardware/sensors/sensor.h"

#include <string.h>

#include "hardware/flash.h"
#include "hardware/push_button.h"
#include "hardware/led_shifter.h"
#include "uart_controller/uart_sensor_controller.h"
#include "sensor_uart_control_core_1.h"
#include "sensor_multicore/sensor_multicore_utils.h"
#include "sensor_definitions.h"
#include "debug_io.h"
#include "utils.h"

#include "pico/multicore.h"
#include "pico/util/queue.h"


const uint ONBOARD_LED_PIN = 25;

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
LEDShifter _ledShifter = {
    .mDataPin = SENSOR_CONNECT_LED_SHIFTER_DATA_PIN,
    .mLatchPin = SENSOR_CONNECT_LED_SHIFTER_LATCH_PIN,
    .mClockPin = SENSOR_CONNECT_LED_SHIFTER_CLOCK_PIN
};


void update_sensor_status_indicators(LEDShifter *ledShifter, Sensor* sensors, uint8_t numSensors) {
    for(int i = 0; i < numSensors; ++i) {
        bool ledOn = (sensors[i].mCurrentSensorData.mSensorStatus == SENSOR_CONNECTED_VALID_DATA);
        set_led_state(ledShifter, sensors[i].mSensorDefinition.mSensorConnectLEDPosition, ledOn);
    }
    output_led_shifter_state(ledShifter);
}


int main() {
    // Initialize debug serial output
    DEBUG_PRINT_INIT();

    // Initialize onboard LED
    gpio_init(ONBOARD_LED_PIN);
    gpio_set_dir(ONBOARD_LED_PIN, GPIO_OUT);
    gpio_put(ONBOARD_LED_PIN, 0);

    // Initialize sensors
    initialize_sensors(
        sensorsList, 
        NUM_SENSORS,
        SENSOR_I2C,
        SENSOR_I2C_BAUDRATE,
        MOISTURE_I2C_SDA,
        MOISTURE_I2C_SCL
    );

    // Initialize status LEDs
    initialize_led_shifter(&_ledShifter);

    // Initialize cross-core queue
    intitialize_sensor_data_queue(&sensorUpdateQueue,(NUM_SENSORS * 4));

    // Initialise UART controller comms interface
    init_sensor_controller(&_sensorControllerInterface, SENSOR_CONTROLLER_TX, SENSOR_CONTROLLER_RX, SENSOR_CONTROLLER_BAUDRATE);

    // Launch secondary core (core 1)
    multicore_launch_core1(sensor_controller_core_main);

    // core0 execution loop
    DEBUG_PRINT("Sensor initialization complete\n");
    while(1) {
        // Update sensor readings
        DEBUG_PRINT("Update sensors\n");
        update_sensor_readings(sensorsList, NUM_SENSORS);

        // Update sensor LED indicators
        DEBUG_PRINT("Update LEDs\n");
        update_sensor_status_indicators(&_ledShifter, sensorsList, NUM_SENSORS);

        // Push sensor updates to core 1
        for(int i = 0; i < NUM_SENSORS; ++i) {
            push_sensor_data_to_queue(&sensorUpdateQueue, &sensorsList[i]);
        }

        // Chill
        sleep_ms(1);
    }
}
