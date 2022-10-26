#include "hardware/sensors/sensor.h"

#include <string.h>
#include <stdio.h>

#include "pico/multicore.h"
#include "hardware/flash.h"
#include "hardware/push_button.h"
#include "hardware/led_shifter.h"
#include "sensor_controller/sensor_controller.h"
#include "sensor_definitions.h"
#include "debug_io.h"
#include "utils.h"


// Serial comms interface to main controller
ControllerInterface _sensorControllerInterface = {
    .mUART = SENSOR_CONTROLLER_UART,
    .mMsgPackSensors = SENSOR_MSGPACK,
    .mNumMsgPackSensors = NUM_SENSORS
};

// Current sensor data
SensorData _currentSensorData[NUM_SENSORS];

// Connection LED controller
LEDShifter _ledShifter = {
    .mDataPin = SENSOR_CONNECT_LED_SHIFTER_DATA_PIN,
    .mLatchPin = SENSOR_CONNECT_LED_SHIFTER_LATCH_PIN,
    .mClockPin = SENSOR_CONNECT_LED_SHIFTER_CLOCK_PIN
};

const uint ONBOARD_LED_PIN = 25;


void update_sensor_status_indicators(LEDShifter *ledShifter, SensorData *sensorData, Sensor** sensors, uint8_t numSensors) {
    for(int i = 0; i < numSensors; ++i) {
        set_led_state(ledShifter, sensors[i]->mSensorConnectLEDPosition, sensorData[i].mIsSensorReadingValid);
    }
    output_led_shifter_state(ledShifter);
}


int main() {
    // Initialize debug serial output
    DEBUG_PRINT_INIT();

    // Turn on onboard LED
    gpio_init(ONBOARD_LED_PIN);
    gpio_set_dir(ONBOARD_LED_PIN, GPIO_OUT);
    gpio_put(ONBOARD_LED_PIN, 0);

    // Initialize sensors
    initialize_sensors(
        _sensorsList, 
        NUM_SENSORS,
        SENSOR_I2C,
        SENSOR_I2C_BAUDRATE,
        MOISTURE_I2C_SDA,
        MOISTURE_I2C_SCL
    );

    // Initialize LEDs
    initialize_led_shifter(&_ledShifter);
    
    // Initialise Controller comms
    init_sensor_controller(&_sensorControllerInterface, SENSOR_CONTROLLER_TX, SENSOR_CONTROLLER_RX, SENSOR_CONTROLLER_BAUDRATE);
    DEBUG_PRINT("Sensor controller serial initialized\n");
    send_controller_ready(&_sensorControllerInterface);

    // Main execution loop
    DEBUG_PRINT("Sensor initialization complete\n");
    while(1) {
                // Process sensor input

        // Update sensor readings
        DEBUG_PRINT("Update sensors\n");
        update_sensor_readings(_sensorsList, _currentSensorData, NUM_SENSORS);


                // Process serial input

        // Handle serial controller I/O
        DEBUG_PRINT("Update serial\n");
        gpio_put(ONBOARD_LED_PIN, 1);
        update_sensor_controller(&_sensorControllerInterface, _sensorsList, _currentSensorData, NUM_SENSORS);
        gpio_put(ONBOARD_LED_PIN, 0);


                // Update physical output

        // Update sensor LED indicators
        DEBUG_PRINT("Update LED\n");
        update_sensor_status_indicators(&_ledShifter, _currentSensorData, _sensorsList, NUM_SENSORS);

        // Chill
        sleep_ms(1);
    }
}
