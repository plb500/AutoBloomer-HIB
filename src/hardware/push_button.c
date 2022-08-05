#include "push_button.h"

#include <stdio.h>

#include "utils.h"

void initialize_push_button(PushButton *pushButton) {
    gpio_init(pushButton->mPin);
    gpio_set_dir(pushButton->mPin, GPIO_IN);
    gpio_pull_up(pushButton->mPin);

    pushButton->mButtonState = 0;
    pushButton->mLastButtonState = true;
    pushButton->mLastDebounceTime = 0;
    pushButton->mButtonPressedTime = 0;
}

PushButtonState update_push_button(PushButton *pushButton) {
    bool pinValue = gpio_get(pushButton->mPin);
    uint32_t currentTime = MILLIS();
    PushButtonState newState = NO_STATE;

    // If the value has changed, reset the debounce timer
    if (pinValue != pushButton->mLastButtonState) {
        pushButton->mLastDebounceTime = currentTime;
    } else {
        // Check to see if the pin has held its value for a decent amount of
        // time
        if ((currentTime - pushButton->mLastDebounceTime) >=
            DEBOUNCE_DELAY_MS) {
            // Looks like it has, we can take this value as the actual reading
            if (pinValue != pushButton->mButtonValue) {
                // The button state has changed so we were either pressed or
                // released
                pushButton->mButtonValue = pinValue;
                if (pinValue) {
                    pushButton->mButtonState = BUTTON_RELEASED;
                    newState = BUTTON_RELEASED;
                } else {
                    pushButton->mButtonState = BUTTON_PRESSED;
                    newState = BUTTON_PRESSED;
                    pushButton->mButtonPressedTime = currentTime;
                }
            } else {
                // The button state hasn't changed, we either want to transition
                // to a held state or a NO_STATE
                switch (pushButton->mButtonState) {
                    case BUTTON_PRESSED:
                        if ((currentTime - pushButton->mButtonPressedTime) >=
                            BUTTON_HOLD_TIME_MS) {
                            pushButton->mButtonState = BUTTON_HELD;
                            newState = BUTTON_HELD;
                        }
                        break;
                    case BUTTON_HELD:
                        break;
                    case BUTTON_RELEASED:
                        pushButton->mButtonState = NO_STATE;
                        newState = NO_STATE;
                        break;
                    case NO_STATE:
                    default:
                        break;
                }
            }
        }
    }

    pushButton->mLastButtonState = pinValue;
    return newState;
}
