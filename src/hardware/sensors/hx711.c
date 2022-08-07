#include "hx711.h"
#include "pico/time.h"
#include "hardware/sync.h"

#include "utils.h"


typedef enum
{
    MSBFIRST,
    LSBFIRST
} BitOrder;

uint8_t shift_in(uint8_t dataPin, uint8_t clockPin, BitOrder bitOrder)
{
    uint8_t value = 0;
    uint8_t pinReading;

    for (int i = 0; i < 8; ++i)
    {
        gpio_put(clockPin, 1);
        sleep_us(1);

        pinReading = gpio_get(dataPin);
        value |= (bitOrder == LSBFIRST) ? (pinReading << i) : (pinReading << (7 - i));

        gpio_put(clockPin, 0);
        sleep_us(1);
    }

    return value;
}

void hx711_init(HX711 *hx711)
{
    gpio_init(hx711->mPDSCK);
    gpio_init(hx711->mSDA);

    gpio_set_dir(hx711->mPDSCK, GPIO_OUT);
    gpio_set_dir(hx711->mSDA, GPIO_IN);

    gpio_pull_up(hx711->mSDA);
}

bool hx711_is_ready(HX711 *hx711)
{
    return gpio_get(hx711->mSDA) == 0;
}

void hx711_wait_ready(HX711 *hx711, uint32_t delayMS)
{
    // Wait for the chip to become ready.
    // Blocks until a load cell is connected.
    while (!hx711_is_ready(hx711))
    {
        sleep_ms(delayMS);
    }
}

bool hx711_wait_ready_retry(HX711 *hx711, int retries, uint32_t delayMS)
{
    // Wait for the chip to become ready by
    // retrying for a specified amount of attempts.
    // https://github.com/bogde/HX711/issues/76
    int count = 0;
    while (count < retries)
    {
        if (hx711_is_ready(hx711))
        {
            return true;
        }
        sleep_ms(delayMS);
        ++count;
    }
    return false;
}

bool hx711_wait_ready_timeout(HX711 *hx711, uint32_t timeout, uint32_t delayMS)
{
    // Wait for the chip to become ready until timeout.
    // https://github.com/bogde/HX711/pull/96
    unsigned long millisStarted = MILLIS();
    while (MILLIS() - millisStarted < timeout)
    {
        if (hx711_is_ready(hx711))
        {
            return true;
        }
        tight_loop_contents();
    }
    return false;
}

int32_t hx711_read(HX711 *hx711)
{
    // Wait for the chip to become ready.
    hx711_wait_ready_timeout(hx711, 20, 1);

    // Define structures for reading data into.
    uint32_t value = 0;
    uint8_t data[3] = {0};
    uint8_t filler = 0x00;

    // Protect the read sequence from system interrupts.  If an interrupt occurs during
    // the time the PD_SCK signal is high it will stretch the length of the clock pulse.
    // If the total pulse time exceeds 60 uSec this will cause the HX711 to enter
    // power down mode during the middle of the read sequence.  While the device will
    // wake up when PD_SCK goes low again, the reset starts a new conversion cycle which
    // forces DOUT high until that cycle is completed.
    //
    // The result is that all subsequent bits read by shiftIn() will read back as 1,
    // corrupting the value returned by read().  The ATOMIC_BLOCK macro disables
    // interrupts during the sequence and then restores the interrupt mask to its previous
    // state after the sequence completes, insuring that the entire read-and-gain-set
    // sequence is not interrupted.  The macro has a few minor advantages over bracketing
    // the sequence between `noInterrupts()` and `interrupts()` calls.
    uint32_t flags = save_and_disable_interrupts();

    // Pulse the clock pin 24 times to read the data.
    data[2] = shift_in(hx711->mSDA, hx711->mPDSCK, MSBFIRST);
    data[1] = shift_in(hx711->mSDA, hx711->mPDSCK, MSBFIRST);
    data[0] = shift_in(hx711->mSDA, hx711->mPDSCK, MSBFIRST);

    // Set the channel and the gain factor for the next reading using the clock pin.
    for (unsigned int i = 0; i < hx711->mGain; i++)
    {
        gpio_put(hx711->mPDSCK, 1);
        sleep_us(1);
        gpio_put(hx711->mPDSCK, 0);
        sleep_us(1);
    }

    restore_interrupts(flags);

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if (data[2] & 0x80)
    {
        filler = 0xFF;
    }
    else
    {
        filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = (((uint32_t)filler) << 24 |
             ((uint32_t)data[2]) << 16 |
             ((uint32_t)data[1]) << 8 |
             ((uint32_t)data[0]));

    return (int32_t)value;
}

int32_t hx711_read_average(HX711 *hx711, uint8_t times)
{
    int32_t sum = 0;
    for (uint8_t i = 0; i < times; i++)
    {
        sum += hx711_read(hx711);
        sleep_ms(0);
    }
    return sum ? (sum / times) : sum;
}

int32_t hx711_get_value(HX711 *hx711, uint8_t times)
{
    int32_t readValue = hx711_read_average(hx711, times);
    return  readValue - hx711->mOffset;
}

float hx711_get_units(HX711 *hx711, uint8_t times)
{
    float value = (float) hx711_get_value(hx711, times); 
    return value * hx711->mScale;
}

float calibrate_to_value(HX711 *hx711, uint8_t times, float calibrated_value) {
    float value = (float) hx711_get_value(hx711, times); 
    hx711->mScale = (calibrated_value / value);

    hx711->mNeedsSaving = true;

    return hx711->mScale;
}

void hx711_tare(HX711 *hx711, uint8_t times)
{
    hx711->mOffset = hx711_read_average(hx711, times);
    hx711->mNeedsSaving = true;
}

void hx711_power_down(HX711 *hx711)
{
    gpio_put(hx711->mPDSCK, 0);
    gpio_put(hx711->mPDSCK, 1);
}

void hx711_power_up(HX711 *hx711)
{
    gpio_put(hx711->mPDSCK, 0);
}
