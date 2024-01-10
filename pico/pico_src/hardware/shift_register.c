#include "shift_register.h"

void init_shift_register(ShiftRegister *shiftRegister) {
    // Initialize pins
    gpio_init(shiftRegister->mLatchPin);
    gpio_set_dir(shiftRegister->mLatchPin, GPIO_OUT);

    gpio_init(shiftRegister->mClockPin);
    gpio_set_dir(shiftRegister->mClockPin, GPIO_OUT);

    gpio_init(shiftRegister->mDataPin);

    switch(shiftRegister->mType) {
        case PISO_SHIFT_REGISTER:
            gpio_set_dir(shiftRegister->mDataPin, GPIO_IN);
            break;
        case SIPO_SHIFT_REGISTER:
            gpio_set_dir(shiftRegister->mDataPin, GPIO_OUT);
            break;
    }

    reset_shift_register(shiftRegister);
}

void reset_shift_register(ShiftRegister *shiftRegister) {
    shiftRegister->mCurrentValue = 0;
}

void set_shift_register_state(ShiftRegister *shiftRegister, uint8_t pos, bool on) {
    if(!shiftRegister || (shiftRegister->mType != SIPO_SHIFT_REGISTER) || (pos >= shiftRegister->mNumBits)) {
        return;
    }

    if(on) {
        shiftRegister->mCurrentValue |= (1 << pos);
    } else {
        shiftRegister->mCurrentValue &= ~(1 << pos);
    }
}

void set_shift_register_states(ShiftRegister *shiftRegister, uint32_t states) {
    if(!shiftRegister || (shiftRegister->mType != SIPO_SHIFT_REGISTER)) {
        return;
    }

    shiftRegister->mCurrentValue = states;
}

void write_shift_register_states(ShiftRegister *shiftRegister) {
    if(!shiftRegister || (shiftRegister->mType != SIPO_SHIFT_REGISTER)) {
        return;
    }

    // Set latch pin low while we are setting the data
    gpio_put(shiftRegister->mLatchPin, 0);
    sleep_us(1);

    // Clock each bit out individually
    for(int bit = (shiftRegister->mNumBits - 1); bit >= 0; --bit) {
        gpio_put(shiftRegister->mClockPin, 0);
        sleep_us(1);
        gpio_put(shiftRegister->mDataPin, (shiftRegister->mCurrentValue & (1 << bit)));
        sleep_us(1);
        gpio_put(shiftRegister->mClockPin, 1);
        sleep_us(1);
    }

    // Set latch high to store data
    gpio_put(shiftRegister->mLatchPin, 1);
}


void read_shift_register_states(ShiftRegister *shiftRegister) {
    if(!shiftRegister || (shiftRegister->mType != PISO_SHIFT_REGISTER)) {
        return;
    }

    shiftRegister->mCurrentValue = 0;

    // Latch the register
    gpio_put(shiftRegister->mLatchPin, false);
    sleep_us(1);
    gpio_put(shiftRegister->mLatchPin, true);
    sleep_us(1);

    // Shift input bits
    for(int i = (shiftRegister->mNumBits - 1); i >= 0; --i) {
        if(gpio_get(shiftRegister->mDataPin)) {
            shiftRegister->mCurrentValue |= (1 << i);
        }

        // Shift next bit
        gpio_put(shiftRegister->mClockPin, true);
        sleep_us(1);
        gpio_put(shiftRegister->mClockPin, false);
        sleep_us(1);
    }    
}


bool get_shift_register_state(ShiftRegister *shiftRegister, uint16_t pos) {
    if(!shiftRegister || (shiftRegister->mType != PISO_SHIFT_REGISTER)) {
        return false;
    }

    return !(shiftRegister->mCurrentValue & (1 << pos));
}
