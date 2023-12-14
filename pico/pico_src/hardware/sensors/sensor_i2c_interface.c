#include "sensor_i2c_interface.h"
#include "hardware/gpio.h"

#define DEFAULT_I2C_TIMEOUT_MS      (100)
const bool I2C_NOSTOP = false;

void init_sensor_bus(I2CInterface *i2cInterface) {
    if(!i2cInterface) {
        return;
    }

    i2c_init(i2cInterface->mI2C, i2cInterface->mBaud);
    gpio_set_function(i2cInterface->mSDA, GPIO_FUNC_I2C);
    gpio_set_function(i2cInterface->mSCL, GPIO_FUNC_I2C);

    gpio_pull_up(i2cInterface->mSDA);
    gpio_pull_up(i2cInterface->mSCL);

    init_shift_register(&i2cInterface->mChannelConnectRegister);
}

void shutdown_sensor_bus(I2CInterface *i2cInterface) {
    if(!i2cInterface) {
        return;
    }

    i2c_deinit(i2cInterface->mI2C);
}

void reset_sensor_bus(I2CInterface *i2cInterface) {
    shutdown_sensor_bus(i2cInterface);
    init_sensor_bus(i2cInterface);
}

void update_connection_status(I2CInterface *i2cInterface) {
    read_shift_register_states(&i2cInterface->mChannelConnectRegister);
}

bool is_i2c_channel_connected(I2CInterface *i2cInterface, I2CChannel channel) {
    if(channel == NO_I2C_CHANNEL) {
        return false;
    }

    return get_shift_register_state(&i2cInterface->mChannelConnectRegister, channel);
}

I2CResponse reset_i2c_multiplexer(I2CInterface *i2cInterface) {
    if(i2cInterface->mMultiplexerAddress < 0) {
        return I2C_RESPONSE_COMMAND_FAILED;
    }

    uint8_t data = 0;

    return write_i2c_data(
        i2cInterface,
        i2cInterface->mMultiplexerAddress,
        &data,
        1
    );
}

I2CResponse select_i2c_channel(I2CInterface *i2cInterface, I2CChannel channel) {
    if(i2cInterface->mMultiplexerAddress < 0) {
        return I2C_RESPONSE_COMMAND_FAILED;
    }

    if(channel == NO_I2C_CHANNEL) {
        return I2C_RESPONSE_COMMAND_FAILED;
    }

    uint8_t data = (uint8_t) (1 << channel);

    return write_i2c_data(
        i2cInterface,
        i2cInterface->mMultiplexerAddress,
        &data,
        1
    );
}

I2CResponse check_i2c_address(I2CInterface *i2cInterface, const uint8_t address) {
    absolute_time_t timeout = make_timeout_time_ms(DEFAULT_I2C_TIMEOUT_MS);

    int response = i2c_write_blocking_until(
        i2cInterface->mI2C, 
        address,
        0,
        0,
        I2C_NOSTOP,
        timeout
    );

    switch(response) {
        case PICO_ERROR_GENERIC:
            return I2C_RESPONSE_ERROR;

        case PICO_ERROR_TIMEOUT:
            return I2C_RESPONSE_TIMEOUT;

        default:
            return I2C_RESPONSE_OK;
    }
}

I2CResponse write_i2c_data(
    I2CInterface *i2cInterface, 
    const uint8_t address, 
    const uint8_t *buffer, 
    size_t bufferLen 
) {
    absolute_time_t timeout = make_timeout_time_ms(DEFAULT_I2C_TIMEOUT_MS);

    // Write the data itself, if we have any
    if(buffer && bufferLen) {
        int response = i2c_write_blocking_until(i2cInterface->mI2C, address, buffer, bufferLen, I2C_NOSTOP, timeout);

        switch(response) {
            case PICO_ERROR_GENERIC:
                return I2C_RESPONSE_ERROR;

            case PICO_ERROR_TIMEOUT:
                return I2C_RESPONSE_TIMEOUT;

            default:
                return (response == bufferLen) ? I2C_RESPONSE_OK : I2C_RESPONSE_INCOMPLETE;
        }
    }

    return I2C_RESPONSE_OK;
}

I2CResponse write_prefixed_i2c_data(
    I2CInterface *i2cInterface, 
    const uint8_t address, 
    const uint8_t *prefixBuffer, 
    size_t prefixLen,
    const uint8_t *buffer, 
    size_t bufferLen 
) {
    // Write the prefix data (usually an address)
    if ((prefixLen != 0) && (prefixBuffer != NULL)) {
        // Again, since we don't want to relinquish the I2C bus we won't bother with the STOP
        absolute_time_t timeout = make_timeout_time_ms(DEFAULT_I2C_TIMEOUT_MS);
        int response = i2c_write_blocking_until(i2cInterface->mI2C, address, prefixBuffer, prefixLen, I2C_NOSTOP, timeout);

        switch(response) {
            case PICO_ERROR_GENERIC:
                return I2C_RESPONSE_ERROR;

            case PICO_ERROR_TIMEOUT:
                return I2C_RESPONSE_TIMEOUT;

            default:
                if(response != prefixLen) return I2C_RESPONSE_INCOMPLETE;
        }
    }

    if(buffer) {
        return write_i2c_data(i2cInterface, address, buffer, bufferLen);
    }

    return I2C_RESPONSE_OK;
}

I2CResponse write_to_i2c_register(
    I2CInterface *i2cInterface, 
    const uint8_t address, 
    const uint8_t regHigh, 
    const uint8_t regLow, 
    const uint8_t *buffer, 
    const uint8_t bufferLen
) {
    uint8_t prefix[] = {
        regHigh,
        regLow
    };

    return write_prefixed_i2c_data(i2cInterface, address, prefix, 2, buffer, bufferLen);
}

I2CResponse read_from_i2c(
    I2CInterface *i2cInterface,
    const uint8_t address,
    uint8_t *buffer, 
    const uint8_t amountToRead
) {
    absolute_time_t timeout = make_timeout_time_ms(DEFAULT_I2C_TIMEOUT_MS);
    int response = i2c_read_blocking_until(i2cInterface->mI2C, address, buffer, amountToRead, I2C_NOSTOP, timeout);

    switch(response) {
        case PICO_ERROR_GENERIC:
            return I2C_RESPONSE_ERROR;

        case PICO_ERROR_TIMEOUT:
            return I2C_RESPONSE_TIMEOUT;

        default:
            return I2C_RESPONSE_OK;
    }
}

I2CResponse read_from_i2c_register(
    I2CInterface *i2cInterface,
    const uint8_t address,
    const uint8_t regHigh, 
    const uint8_t regLow,
    uint8_t *buffer, 
    const uint8_t amountToRead, 
    const uint16_t readDelay
) {
    uint8_t pos = 0;

    // Write register/command data
    I2CResponse registerResponse = write_to_i2c_register(i2cInterface, address, regHigh, regLow, 0, 0);
    if(registerResponse != I2C_RESPONSE_OK) {
        return registerResponse;
    }

    // Wait for response
    sleep_ms(readDelay);

    // Read response
    return read_from_i2c(i2cInterface, address, buffer, amountToRead);
}
