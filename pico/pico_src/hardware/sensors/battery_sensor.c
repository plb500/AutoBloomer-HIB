#include "battery_sensor.h"
#include "hardware/adc.h"

// Max value will be 3.3v (internal ADC ref). Convert from 12-bit value to voltage.
// Voltage is put through a voltage divider which halves the voltage
// const float ADC_CONVERSION_FACTOR = (6.6f / (1 << 12));
const float ADC_CONVERSION_FACTOR = 0.001664876476907;      // Got this via actually measuring the pin voltage. There's some drop over the voltage divider

void initialize_battery_sensor(BatteryVoltageSensor *sensor) {
    adc_init();
    adc_gpio_init(sensor->mBatterySensePin);
    adc_select_input(sensor->mADCInput);

    sensor->mCurrentVoltage = 0.f;
 }

void update_battery_level(BatteryVoltageSensor *sensor) {
    uint16_t adc_value = adc_read();
    sensor->mCurrentVoltage = (adc_value * ADC_CONVERSION_FACTOR);
}
