#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "pico/stdlib.h"

typedef enum {
    NO_ERROR,
    PULLUP_TRANSITION_TIMEOUT,
    LOW_INIT_TIMEOUT,
    START_TRANSMISSION_TIMEOUT,
    START_BIT_HEADER_TIMEOUT,
    BIT_VALUE_TIMEOUT,
    BIT_DATA_TIMEOUT,
    CHECKSUM_ERROR
} DHT22Error;

typedef struct {
    float mRelativeHumidity;
    float mTemperatureC;
    DHT22Error mReadingError;
} DHT22Data;

typedef struct {
    const uint8_t mDataPin;
    uint32_t mLastReadTime;
    float mLastHumidityReading;
    float mLastTemperatureReading;
} DHT22Sensor;

static const uint32_t DHT22_SAMPLE_RATE_DELAY_MS = 2500;        // Sample once every 2.5s

void init_DHT22(DHT22Sensor *sensor);
void read_DHT22(DHT22Sensor *sensor, DHT22Data *readings);

#endif  //  DHT_SENSOR_H
