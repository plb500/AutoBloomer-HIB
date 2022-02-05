#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H

#include "pico/stdlib.h"

typedef enum {
    NO_STATE = 0,
    BUTTON_PRESSED = 1,
    BUTTON_HELD = 2,
    BUTTON_RELEASED = 3
} PushButtonState;

typedef struct {
    const uint mPin;
    PushButtonState mButtonState;
    bool mButtonValue;
    bool mLastButtonState;
    uint32_t mLastDebounceTime;
    uint32_t mButtonPressedTime;
} PushButton;

static const uint32_t DEBOUNCE_DELAY_MS = 50;
static const uint32_t BUTTON_HOLD_TIME_MS = 1500;

void initialize_push_button(PushButton *pushButton);
PushButtonState update_push_button(PushButton *pushButton);

#endif  // PUSH_BUTTON_H