#ifndef HX711_H
#define HX711_H

#include "pico/stdlib.h"

typedef enum
{
    GAIN_FACTOR_128 = 1, // Channel A, gain factor 128
    GAIN_FACTOR_64 = 3,  // Channel A, gain factor 64
    GAIN_FACTOR_32 = 2   // Channel B, gain factor 32
} HX711GainFactor;

typedef struct
{
    uint8_t mPDSCK;         // Power Down and Serial Clock Pin
    uint8_t mSDA;           // Serial Data Pin
    HX711GainFactor mGain;  // Amplification factor
    int32_t mOffset;        // Used for tare weight
    float mScale;           // Used to return weight
} HX711;

// Initialize library with data output pin, clock input pin and gain factor.
// Channel selection is made by passing the appropriate gain:
// - With a gain factor of 64 or 128, channel A is selected
// - With a gain factor of 32, channel B is selected
// The library default is "128" (Channel A).
void hx711_init(HX711 *hx711);

// Check if HX711 is ready
// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
bool hx711_is_ready(HX711 *hx711);

// Wait for the HX711 to become ready
void hx711_wait_ready(HX711 *hx711, uint32_t delayMS);
bool hx711_wait_ready_retry(HX711 *hx711, int retries, uint32_t delayMS);
bool hx711_wait_ready_timeout(HX711 *hx711, uint32_t timeout, uint32_t delayMS);

// waits for the chip to be ready and returns a reading
int32_t hx711_read(HX711 *hx711);

// returns an average reading; times = how many times to read
int32_t hx711_read_average(HX711 *hx711, uint8_t times);

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
int32_t hx711_get_value(HX711 *hx711, uint8_t times);

// returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
// times = how many readings to do
float hx711_get_units(HX711 *hx711, uint8_t times);

// set the OFFSET value for tare weight; times = how many times to read the tare value
void hx711_tare(HX711 *hx711, uint8_t times);

// puts the chip into power down mode
void hx711_power_down(HX711 *hx711);

// wakes up the chip after power down mode
void hx711_power_up(HX711 *hx711);

#endif // HX711_H
