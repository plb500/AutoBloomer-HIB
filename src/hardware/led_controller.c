#include "led_controller.h"

#include "hardware/gpio.h"
#include "hardware_definitions.h"

void initialize_led_controller(LEDController *ledController,
                               bool initialState) {
    gpio_init(ledController->mPin);
    gpio_set_dir(ledController->mPin, GPIO_OUT);

    ledController->mState = initialState;
    gpio_put(ledController->mPin, initialState);
}

void update_led_controller(LEDController *ledController) {
    if (ledController->mIsFlashing) {
        uint32_t currentTime = MILLIS();
        uint32_t timeSinceFlashStart =
            (currentTime - ledController->mFlashStartTime);
        if (timeSinceFlashStart >= ledController->mFlashDurationMS) {
            gpio_put(ledController->mPin, ledController->mState);
            ledController->mIsFlashing = false;
        } else {
            uint32_t flashPeriod =
                (timeSinceFlashStart / ledController->mFlashPeriodMS);
            bool flashValue = ((flashPeriod % 2) == 1);
            if (flashValue != gpio_get(ledController->mPin)) {
                gpio_put(ledController->mPin, flashValue);
            }
        }
    } else {
        if (gpio_get(ledController->mPin) != ledController->mState) {
            gpio_put(ledController->mPin, ledController->mState);
        }
    }
}

void flash_led_controller(LEDController *ledController, uint32_t flashPeriodMS,
                          uint32_t flashDurationMS) {
    ledController->mFlashStartTime = MILLIS();
    ledController->mIsFlashing = true;
    ledController->mFlashPeriodMS = flashPeriodMS;
    ledController->mFlashDurationMS = flashDurationMS;
}
