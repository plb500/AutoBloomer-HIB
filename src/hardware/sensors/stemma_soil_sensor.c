#include "stemma_soil_sensor.h"
#include "hardware/gpio.h"

#include <stdio.h>


const uint8_t SEESAW_STATUS_BASE = 0x00;
const uint8_t SEESAW_STATUS_HW_ID = 0x01;
const uint8_t SEESAW_STATUS_SWRST = 0x7F;
const uint8_t SEESAW_HW_ID_CODE = 0x55;
const uint8_t SEESAW_STATUS_VERSION = 0x02;
const uint8_t SEESAW_TOUCH_BASE = 0x0F;
const uint8_t SEESAW_TOUCH_CHANNEL_OFFSET = 0x10;


bool i2c_detect_soil_sensor(StemmaSoilSensor *soilSensor) {
    if (i2c_write_blocking(soilSensor->mI2C, soilSensor->mAddress, 0, 0, true) == PICO_ERROR_GENERIC) {
        return false;
    }
    return true;
}

bool i2c_write_data_with_prefix(StemmaSoilSensor *soilSensor, const uint8_t *buffer, size_t len, bool stop, const uint8_t *prefixBuffer, size_t prefixLen) {
    // Write the prefix data (usually an address)
    if ((prefixLen != 0) && (prefixBuffer != NULL)) {
        int written = i2c_write_blocking(soilSensor->mI2C, soilSensor->mAddress, prefixBuffer, prefixLen, stop); 
        if (written != prefixLen) {
            return false;
        }
    }

    // Write the data itself, if we have any
    if(buffer && len) {
        return (i2c_write_blocking(soilSensor->mI2C, soilSensor->mAddress, buffer, len, stop) != len);
    }

    return true;
}

bool i2c_write_soil_sensor_register(StemmaSoilSensor *soilSensor, const uint8_t regHigh, const uint8_t regLow, const uint8_t *buf, const uint8_t num) {
    uint8_t prefix[] = {
        regHigh,
        regLow
    };

    return i2c_write_data_with_prefix(soilSensor, buf, num, true, prefix, 2);
}

bool i2c_read_from_register(StemmaSoilSensor *soilSensor, const uint8_t regHigh, const uint8_t regLow, 
                            uint8_t *buf, const uint8_t num, const uint16_t delay) {
    uint8_t pos = 0;

    // Write register/command data
    if(!i2c_write_soil_sensor_register(soilSensor, regHigh, regLow, 0, 0)) {
        return false;
    }

    // Wait for response
    sleep_ms(delay);

    // Read response
    uint16_t amountRead = i2c_read_blocking(soilSensor->mI2C, soilSensor->mAddress, buf, num, false);
    return (amountRead > 0);
}


void init_sensor_bus(i2c_inst_t *i2c, const int baud, const int sdaPin, const int sclPin) {
    i2c_init(i2c, baud);
    gpio_set_function(sdaPin, GPIO_FUNC_I2C);
    gpio_set_function(sclPin, GPIO_FUNC_I2C);

    gpio_pull_up(sdaPin);
    gpio_pull_up(sclPin);
}

bool soil_sensor_begin(StemmaSoilSensor *soilSensor) {
    uint8_t response = 0xFF;

    // Scan bus for device at given address
    bool found = false;
    for (int retries = 0; retries < 10; retries++) {
        if (i2c_detect_soil_sensor(soilSensor)) {
            found = true;
            break;
        }
        sleep_ms(10);
    }

    if (!found) {
        return false;
    }

    // Reset device
    reset_soil_sensor(soilSensor);
    found = false;
    for (int retries = 0; retries < 10; retries++) {
        if (i2c_detect_soil_sensor(soilSensor)) {
            found = true;
            break;
        }
        sleep_ms(10);
    }

    if (!found) {
        return false;
    }

    // Get hardware ID from device
    found = false;
    for (int retries = 0; !found && retries < 10; retries++) {
        i2c_read_from_register(soilSensor, SEESAW_STATUS_BASE, SEESAW_STATUS_HW_ID, &response, 1, 2);
        if(response == SEESAW_HW_ID_CODE) {
            found = true;
            break;
        }
    }

    return found;
}

bool reset_soil_sensor(StemmaSoilSensor *soilSensor) {
    const uint8_t resetBuffer[] = {
        0xFF
    };
    return i2c_write_soil_sensor_register(soilSensor, SEESAW_STATUS_BASE, SEESAW_STATUS_SWRST, resetBuffer, 1);
}

uint get_soil_sensor_version(StemmaSoilSensor *soilSensor) {
    uint8_t buf[4];
    i2c_read_from_register(soilSensor, SEESAW_STATUS_BASE, SEESAW_STATUS_VERSION, buf, 4, 100);

    return (((uint) buf[0] << 24) | 
            ((uint) buf[1] << 16) |
            ((uint) buf[2] << 8) |
            (uint) buf[3]);
}

uint16_t get_soil_sensor_capacitive_value(StemmaSoilSensor *soilSensor) {
    static const uint16_t READ_DELAY_MS = 5;
    static const uint16_t NUM_RETRIES = 3;
    static const int READING_BUFFER_SIZE = 2;
    
    uint8_t buf[READING_BUFFER_SIZE];
    uint16_t ret = STEMMA_SOIL_SENSOR_INVALID_READING;

    for(uint8_t retry = 0; retry < NUM_RETRIES; retry++) {
        if(i2c_read_from_register(soilSensor, SEESAW_TOUCH_BASE, SEESAW_TOUCH_CHANNEL_OFFSET, buf, READING_BUFFER_SIZE, READ_DELAY_MS)) {
            ret = ((uint16_t) buf[0] << 8) | buf[1];
        }
    }
    return ret;
}
