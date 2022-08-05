#include "hardware/sensors/sensor.h"

#include "pico/multicore.h"
#include "hardware/flash.h"
#include "hardware/push_button.h"
#include "sensor_controller/sensor_controller.h"
#include "sensor_definitions.h"

#include <string.h>
#include <stdio.h>


// Push buttons for taring load sensors
PushButton _tareButtonL = { LOAD_SENSOR_TARE_L };
PushButton _tareButtonR = { LOAD_SENSOR_TARE_R };

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
        LOAD_SENSOR_L_ACTIVE_LED,
    },
    {
        LOAD_SENSOR_R_ACTIVE_LED,
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


// Load sensor persistence values
static const uint32_t VALID_TARE_PERSISTENCE_DATA = 0xDEADBEEF;
typedef struct {
    uint32_t mPersistenceFlag;
    float mTareValues[NUM_SENSORS];
    uint32_t mWriteTime;
} TarePersistence;

// We're going to erase and reprogram a region 256k from the start of flash.
// Once done, we can access this at XIP_BASE + 256k.
#define FLASH_TARGET_OFFSET             (256 * 1024)
const uint8_t *_flashTargetContents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

// We'll have a static buffer to write into temporarily during the 
// sensor update and then copy to flash
#define NUM_TARE_FLASH_PAGES            ((sizeof(TarePersistence) / FLASH_PAGE_SIZE) + 1)
#define TARE_PERSISTENCE_BUFFER_SIZE    (NUM_TARE_FLASH_PAGES * FLASH_PAGE_SIZE)
#define NUM_TARE_FLASH_SECTORS          ((TARE_PERSISTENCE_BUFFER_SIZE / FLASH_SECTOR_SIZE) + 1)
uint8_t _tarePersistenceWriteBuffer[TARE_PERSISTENCE_BUFFER_SIZE];


void update_sensor_status_indicators(LEDController *ledControllers, SensorData *sensorData, uint8_t numSensors) {
    for(int i = 0; i < numSensors; ++i) {
        ledControllers[i].mState = sensorData[i].mIsSensorReadingValid;
        update_led_controller(&ledControllers[i]);
    }
}

void retrieve_tare_values(Sensor **sensors, int numSensors) {
    TarePersistence *tp = (TarePersistence*) _flashTargetContents;

    DEBUG_PRINT("Last persistence write time: %u\n", tp->mWriteTime);

    for(int i = 0; i < numSensors; ++i) {
        if(sensors[i]->mSensorType == LOAD_SENSOR) {
            float tareOffset = (tp->mPersistenceFlag == VALID_TARE_PERSISTENCE_DATA) ? tp->mTareValues[i] : 0.f;
            sensors[i]->mSensor.mHX711.mOffset = tareOffset;
        }
    }
}

void persist_tare_values(Sensor **sensors, int numSensors) {
    TarePersistence *tp = (TarePersistence*) _tarePersistenceWriteBuffer;
    tp->mPersistenceFlag = VALID_TARE_PERSISTENCE_DATA;
    tp->mWriteTime = MILLIS();

    for(int i = 0; i < numSensors; ++i) {
        if(sensors[i]->mSensorType == LOAD_SENSOR) {
            tp->mTareValues[i] = sensors[i]->mSensor.mHX711.mOffset;
        } else {
            tp->mTareValues[i] = 0.f;
        }
    }

    // Copy tare persistence data into flash.
    // For safety, we should not be interrupted during the write,
    // so disable interrupts 
    uint32_t status = save_and_disable_interrupts();

    // Start by wiping the persistence region
    // Note that a whole number of sectors must be erased at a time.
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE * NUM_TARE_FLASH_SECTORS);

    // Program the target region. Can only program multiples of 256 bytes (one page)
    flash_range_program(FLASH_TARGET_OFFSET, _tarePersistenceWriteBuffer, TARE_PERSISTENCE_BUFFER_SIZE);

    // We are done - we can re-enable interrupts
    restore_interrupts(status);

    DEBUG_PRINT("Wrote persistence values. Write time: %u\n", tp->mWriteTime);
}

int main() {
    PushButtonState buttonStateL, buttonStateR;
    bool persistenceDataDirty = false;

    // Initialize debug serial output
    DEBUG_PRINT_INIT();

    // Initialize persistence data
    memset(_tarePersistenceWriteBuffer, 0, TARE_PERSISTENCE_BUFFER_SIZE);

    // Initialize tare buttons
    initialize_push_button(&_tareButtonL);
    initialize_push_button(&_tareButtonR);
    
    // Initialize sensors
    initialize_sensors(_sensorsList, NUM_SENSORS);
    retrieve_tare_values(_sensorsList, NUM_SENSORS);

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
        persistenceDataDirty = false;

                // Process physical input

        // Check for load sensor tare
        buttonStateL = update_push_button(&_tareButtonL);
        buttonStateR = update_push_button(&_tareButtonR);
        if((buttonStateL == BUTTON_HELD) || (buttonStateR == BUTTON_HELD)) {
            if(buttonStateL == BUTTON_HELD) {
                DEBUG_PRINT("Taring left load sensor\n");
                flash_led_controller(&_ledControllers[LOAD_SENSOR_L_ID], SLOW_FLASH_PERIOD_MS, DEFAULT_FLASH_DURATION_MS);
                hx711_tare(&_sensorsList[LOAD_SENSOR_L_ID]->mSensor.mHX711, 10);
            }

            if(buttonStateR == BUTTON_HELD) {
                DEBUG_PRINT("Taring right load sensor\n");
                flash_led_controller(&_ledControllers[LOAD_SENSOR_R_ID], SLOW_FLASH_PERIOD_MS, DEFAULT_FLASH_DURATION_MS);
                hx711_tare(&_sensorsList[LOAD_SENSOR_R_ID]->mSensor.mHX711, 10);
            }
            
            persistenceDataDirty = true;
        }

                // Process sensor input

        // Update sensor readings
        update_sensor_readings(_sensorsList, _currentSensorData, NUM_SENSORS);


                // Process serial input

        // Handle serial controller I/O
        update_sensor_controller(&_sensorControllerInterface, _currentSensorData, NUM_SENSORS);


                // Update physical output

        // Update sensor LED indicators
        update_sensor_status_indicators(_ledControllers, _currentSensorData, NUM_SENSORS);

        // Persist data if required
        if(persistenceDataDirty) {
            DEBUG_PRINT("Persisting tare values to flash\n");
            persist_tare_values(_sensorsList, NUM_SENSORS);
            DEBUG_PRINT("Done\n");
        }

        // Chill?
        sleep_ms(1);
    }
}
