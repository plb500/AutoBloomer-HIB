#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "pico/stdlib.h"

static const uint32_t FAST_FLASH_PERIOD_MS = 200;
static const uint32_t SLOW_FLASH_PERIOD_MS = 400;
static const uint32_t DEFAULT_FLASH_DURATION_MS = 2000;

typedef struct {
    const uint mPin;
    bool mState;
    bool mIsFlashing;
    uint32_t mFlashStartTime;
    uint32_t mFlashPeriodMS;
    uint32_t mFlashDurationMS;
} LEDController;

void initialize_led_controller(LEDController *ledController, bool initialState);
void update_led_controller(LEDController *ledController);
void flash_led_controller(LEDController *ledController, uint32_t flashPeriodMS, uint32_t flashDurationMS);

#endif  // LED_CONTROLLER_H
