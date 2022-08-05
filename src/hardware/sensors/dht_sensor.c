#include "dht_sensor.h"

#include <stdio.h>

#include "utils.h"

static inline int wait_for_pin_value(int pin, int value, int timeout)
{
    while ((gpio_get(pin) != value) && timeout)
    {
        sleep_us(1);
        --timeout;
    }
    return timeout;
}

void init_DHT22(DHT22Sensor *sensor) {
    gpio_init(sensor->mDataPin);
    sensor->mLastReadTime = 0;
}

void read_DHT22(DHT22Sensor *sensor, DHT22Data *readings)
{
    const uint32_t currentTime = MILLIS();
    if((currentTime - sensor->mLastReadTime) < DHT22_SAMPLE_RATE_DELAY_MS) {
        // Not time for sample reading yet
        return;
    }

    sensor->mLastReadTime = currentTime;

    // DHT22 response will contain 5 bytes
    const uint8_t RESPONSE_SIZE = 5;
    uint8_t rawResponse[] = {
        0, // RH integral
        0, // RH fractional
        0, // Temperature integral
        0, // Temperature fractional
        0  // Checksum (RH integral + RH fractional + Temperature integral + Temperature fractional) & 0xFF
    };

    uint8_t currentBitDuration = 0;
    uint8_t currentByte = 0;
    uint8_t currentBit = 0;

    // Let pin transition to known pullup state
    gpio_set_dir(sensor->mDataPin, GPIO_IN);
    if (!wait_for_pin_value(sensor->mDataPin, 1, 10))
    {
        readings->mReadingError = PULLUP_TRANSITION_TIMEOUT;
        return;
    }

    // Trigger reading from DHT by holding communication pin low
    // for minimum of 1ms
    gpio_set_dir(sensor->mDataPin, GPIO_OUT);
    gpio_put(sensor->mDataPin, 0);
    sleep_ms(2);

    // Start reading
    gpio_set_dir(sensor->mDataPin, GPIO_IN);
    if (!wait_for_pin_value(sensor->mDataPin, 1, 10))
    {
        readings->mReadingError = PULLUP_TRANSITION_TIMEOUT;
        return;
    }

    // Wait for DHT22 to pull line low (20-40us)
    if (!wait_for_pin_value(sensor->mDataPin, 0, 50))
    {
        readings->mReadingError = LOW_INIT_TIMEOUT;
        return;
    }

    // Line has been pulled low, signalling start of DHT ack signal
    // Wait for DHT22 to pull line high (80us)
    if (!wait_for_pin_value(sensor->mDataPin, 1, 100))
    {
        readings->mReadingError = START_TRANSMISSION_TIMEOUT;
        return;
    }

    // Line has been pulled high, signalling DHT is preparing to send bit data
    // Wait for DHT22 to pull line low (80us)
    if (!wait_for_pin_value(sensor->mDataPin, 0, 100))
    {
        readings->mReadingError = START_BIT_HEADER_TIMEOUT;
        return;
    }

    while (currentByte < RESPONSE_SIZE)
    {
        // Line pulled low for bit header
        // Wait for DHT to pull line high for bit data (50us)
        if (!wait_for_pin_value(sensor->mDataPin, 1, 75))
        {
            readings->mReadingError = BIT_VALUE_TIMEOUT;
            return;
        }

        // Line pulled high for bit transmission
        // Line high duration indicates bit value
        //     26-28us: Bit is "0"  (this looks to measure out to a count of ~17-18 in reality)
        //        70us: Bit is "1"  (this looks to measure out to a count of ~40 in reality)
        uint8_t timeout = 100;
        currentBitDuration = 0;
        while (gpio_get(sensor->mDataPin) && timeout)
        {
            sleep_us(1);
            ++currentBitDuration;
            --timeout;
        }
        if (!timeout)
        {
            // Hardware error
            readings->mReadingError = BIT_DATA_TIMEOUT;
            return;
        }

        if (currentBitDuration > 35)
        {
            rawResponse[currentByte] |= (1 << (7 - currentBit));
        }

        ++currentBit;
        if (currentBit == 8)
        {
            ++currentByte;
            currentBit = 0;
        }
    }

    // Validate response
    int checksum = 0;
    for (int i = 0; i < RESPONSE_SIZE - 1; ++i)
    {
        checksum += rawResponse[i];
    }
    if ((checksum & 0xFF) != rawResponse[RESPONSE_SIZE - 1])
    {
        // Checksum error
        readings->mReadingError = CHECKSUM_ERROR;
        return;
    }

    // Everything looks good. Convert raw response

    // Humidity
    readings->mRelativeHumidity = (((uint16_t)rawResponse[0] << 8) | rawResponse[1]);
    readings->mRelativeHumidity *= 0.1;

    // Temperature
    readings->mTemperatureC = ((((uint16_t)(rawResponse[2] & 0x7F)) << 8) | rawResponse[3]);
    readings->mTemperatureC *= 0.1;
    if (rawResponse[2] & 0x80)
    {
        readings->mTemperatureC *= -1;
    }

    readings->mReadingError = false;
}
