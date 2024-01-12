#include "battery_sensor.h"
#include "debug_io.h"

#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <math.h>


// Max value will be 3.3v (internal ADC ref). Convert from 12-bit value to voltage.
// Voltage is put through a voltage divider which halves the voltage
const float ADC_CONVERSION_FACTOR = 0.001591464080024;      // Got this via actually measuring the pin voltage. There's some drop over the voltage divider
const uint BATTERY_SAMPLE_PERIOD_MS = 2000;                 // Battery isn't going to be draining rapidly. Can measure once every couple of minutes
const uint BATTERY_CHARGE_PERIOD_MS = 20;                   // Time for capacitor to build up

#define BATTERY_SAMPLE_COUNT_FACTOR     (3)                 // We'll take 8 (2^3) battery readings

void initialize_battery_sensor(BatteryVoltageSensor *sensor) {
    // Initialize the GPIO pin which will trigger battery current into the ADC pin.
    // It is driven LOW to activate the circuit
    gpio_init(sensor->mEnableSensePin);
    gpio_set_dir(sensor->mEnableSensePin, GPIO_OUT);
    gpio_put(sensor->mEnableSensePin, 1);

    // Initialize the ADC
    adc_init();
    adc_gpio_init(sensor->mBatteryMeasurePin);
    adc_select_input(sensor->mADCInput);

    sensor->mCurrentVoltage = 0.f;
    sensor->mCurrentState = BATTERY_SENSOR_SLEEPING;
    sensor->mSensorTransitionTime = make_timeout_time_ms(0);
 }

bool battery_sensor_update(BatteryVoltageSensor *sensor) {
    volatile uint adc_data = 0;
    bool hasReading = false;

    if(!sensor) {
        return hasReading;
    }

    if(absolute_time_diff_us(sensor->mSensorTransitionTime, get_absolute_time()) <= 0) {
        DEBUG_PRINT("      +- No data available currently...\n");
        return hasReading;
    }

    // Handle our transition point
    switch(sensor->mCurrentState) {
        case BATTERY_SENSOR_SLEEPING:
            // Time to start charging the capacitor, enable measurement circuit
            gpio_put(sensor->mEnableSensePin, 0);
            sensor->mSensorTransitionTime = make_timeout_time_ms(BATTERY_CHARGE_PERIOD_MS);
            sensor->mCurrentState = BATTERY_SENSOR_CHARGING;
            break;

        case BATTERY_SENSOR_CHARGING:
            // Time to read the sensor
            for(int i = 0; i < (1 << BATTERY_SAMPLE_COUNT_FACTOR); ++i) {
                adc_data += adc_read();
            }
            adc_data = (adc_data >> BATTERY_SAMPLE_COUNT_FACTOR);

            sensor->mCurrentVoltage = (round(adc_data * ADC_CONVERSION_FACTOR * 100) / 100.f);

            // Disable measurement circuit
            gpio_put(sensor->mEnableSensePin, 1);

            sensor->mSensorTransitionTime = make_timeout_time_ms(BATTERY_SAMPLE_PERIOD_MS);
            sensor->mCurrentState = BATTERY_SENSOR_SLEEPING;
            hasReading = true;
            DEBUG_PRINT("      +- Battery voltage: %.2fv (ADC: %d)\n", sensor->mCurrentVoltage, adc_data);
            break;
    }

    return hasReading;
}
