#include "led_shifter.h"
#include "hardware/gpio.h"

void initialize_led_shifter(LEDShifter *shifter) {
    // Initialize pins
    gpio_init(shifter->mDataPin);
    gpio_set_dir(shifter->mDataPin, GPIO_OUT);

    gpio_init(shifter->mLatchPin);
    gpio_set_dir(shifter->mLatchPin, GPIO_OUT);

    gpio_init(shifter->mClockPin);
    gpio_set_dir(shifter->mClockPin, GPIO_OUT);

    clear_led_states(shifter);
}

void clear_led_states(LEDShifter *shifter) {
    shifter->mLEDStates = 0;
}

void set_led_state(LEDShifter *shifter, uint8_t pos, bool on) {
    if(on) {
        shifter->mLEDStates |= (1 << pos);
    } else {
        shifter->mLEDStates &= ~(1 << pos);
    }
}

void set_led_states(LEDShifter *shifter, uint8_t states) {
    shifter->mLEDStates = states;
}

void output_led_shifter_state(LEDShifter *shifter) {
    // Set latch pin low while we are setting the data
    gpio_put(shifter->mLatchPin, 0);

    // Clock each bit out individually
    for(int bit = 0; bit < 8; ++bit) {
        gpio_put(shifter->mClockPin, 0);
        gpio_put(shifter->mDataPin, (shifter->mLEDStates & (1 << bit)));
        gpio_put(shifter->mClockPin, 1);
    }

    // Set latch high to store data
    gpio_put(shifter->mLatchPin, 1);
}
