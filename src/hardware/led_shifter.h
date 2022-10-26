#ifndef __LED_SHIFTER_H_
#define __LED_SHIFTER_H_

#include <unistd.h>
#include <stdbool.h>

typedef struct {
    const uint mDataPin;
    const uint mLatchPin;
    const uint mClockPin;

    uint8_t mLEDStates;
} LEDShifter;

void initialize_led_shifter(LEDShifter *shifter);
void clear_led_states(LEDShifter *shifter);
void set_led_state(LEDShifter *shifter, uint8_t pos, bool on);
void set_led_states(LEDShifter *shifter, uint8_t states);
void output_led_shifter_state(LEDShifter *shifter);

#endif      // __LED_SHIFTER_H_