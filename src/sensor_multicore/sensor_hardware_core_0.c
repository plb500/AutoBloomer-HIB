#include "hardware/sensors/sensor.h"

#include <string.h>

#include "hardware/flash.h"
#include "hardware/shift_register.h"
#include "hardware/sensors/sensor_i2c_interface.h"

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
ShiftRegister _ledShifter = {
    .mDataPin = SIPO_DATA_PIN,
    .mLatchPin = SIPO_LATCH_PIN,
    .mClockPin = SIPO_CLOCK_PIN,
    .mType = SIPO_SHIFT_REGISTER
};

// I2C bus controller
I2CInterface _i2cInterface = {
    .mI2C = SENSOR_I2C,
    .mBaud = SENSOR_I2C_BAUDRATE,
    .mSDA = SENSOR_I2C_SDA,
    .mSCL = SENSOR_I2C_SCL,
    .mMultiplexerAddress = DEFAULT_MULTIPLEXER_ADDRESS,
    .mChannelConnectRegister = {
        .mDataPin = PISO_DATA_PIN,
        .mLatchPin = PISO_LATCH_PIN,
        .mClockPin = PISO_CLOCK_PIN,
        .mType = PISO_SHIFT_REGISTER
    }
};

void update_sensor_status_indicators(ShiftRegister *shiftRegister, Sensor* sensors, uint8_t numSensors) {
    for(int i = 0; i < numSensors; ++i) {
        bool ledOn = (sensors[i].mCurrentSensorData.mSensorStatus == SENSOR_CONNECTED_VALID_DATA);
        set_shift_register_state(shiftRegister, sensors[i].mSensorDefinition.mSensorConnectLEDPosition, ledOn);
    }
    write_shift_register_states(shiftRegister);
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
        &_i2cInterface
    );

    // Initialize status LEDs
    init_shift_register(&_ledShifter);

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
