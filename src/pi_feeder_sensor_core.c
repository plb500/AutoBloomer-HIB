#include "hardware/sensors/sensor.h"

#include <string.h>
#include <stdio.h>

#include "pico/multicore.h"
#include "hardware/flash.h"
#include "hardware/push_button.h"
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

// LED Display buttons
LEDController _ledControllers[NUM_SENSORS] = {
    {
        SONAR_SENSOR_L_ACTIVE_LED,
    },
    {
        SONAR_SENSOR_R_ACTIVE_LED,
    },
    {
        TEMP_SENSOR_L_ACTIVE_LED,
    },
    {
        TEMP_SENSOR_R_ACTIVE_LED,
    },
    {
        MOISTURE_SENSOR_L_ACTIVE_LED,
    },
    {
        MOISTURE_SENSOR_R_ACTIVE_LED,
    }
};

const uint ONBOARD_LED_PIN = 25;

void update_sensor_status_indicators(LEDController *ledControllers, SensorData *sensorData, uint8_t numSensors) {
    for(int i = 0; i < numSensors; ++i) {
        ledControllers[i].mState = sensorData[i].mIsSensorReadingValid;
        update_led_controller(&ledControllers[i]);
    }
}


int main() {
    // Initialize debug serial output
    DEBUG_PRINT_INIT();

    // Turn on onboard LED
    gpio_init(ONBOARD_LED_PIN);
    gpio_set_dir(ONBOARD_LED_PIN, GPIO_OUT);
    gpio_put(ONBOARD_LED_PIN, 1);

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
    for(int i = 0; i < NUM_SENSORS; ++i) {
        initialize_led_controller(&_ledControllers[i], false);
    }

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
        update_sensor_controller(&_sensorControllerInterface, _sensorsList, _currentSensorData, NUM_SENSORS);


                // Update physical output

        // Update sensor LED indicators
        DEBUG_PRINT("Update LED\n");
        update_sensor_status_indicators(_ledControllers, _currentSensorData, NUM_SENSORS);

        // Chill
        sleep_ms(1);
    }
}
