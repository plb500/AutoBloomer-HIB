#ifndef _SENSOR_I2C_INTERFACE_H_
#define _SENSOR_I2C_INTERFACE_H_

#include "hardware/shift_register.h"
#include "hardware/i2c.h"

#define DEFAULT_MULTIPLEXER_ADDRESS     (0x70)


// I2C multiplexer channel definitions
typedef enum {
    I2C_CHANNEL_0 = 0,
    I2C_CHANNEL_1 = 1,
    I2C_CHANNEL_2 = 2,
    I2C_CHANNEL_3 = 3,
    I2C_CHANNEL_4 = 4,
    I2C_CHANNEL_5 = 5,
    I2C_CHANNEL_6 = 6,
    I2C_CHANNEL_7 = 7,

    NO_I2C_CHANNEL = -1
} I2CChannel;


typedef struct {
    int8_t mMultiplexerAddress;                 // I2C Address pf the multiplexer device
    int8_t mResetPin;                           // For resetting the TCA9548A chip
    ShiftRegister mChannelConnectRegister;      // For detecting which I2C channels are physically connected
} I2CMultiplexer;


typedef struct {
    i2c_inst_t *mI2C;                           // The underlying I2C access struct
    int mBaud;                                  // I2C baud rate
    int mSDA;                                   // I2C SDA pin
    int mSCL;                                   // I2C SCL pin
    I2CMultiplexer *mMultiplexer;      // NULL for no multiplexer (direct I2C connections)
} I2CInterface;


typedef enum {
    I2C_RESPONSE_OK                 = 0,
    I2C_RESPONSE_ERROR              = 1,
    I2C_RESPONSE_TIMEOUT            = 2,
    I2C_RESPONSE_INVALID_REQUEST    = 3,
    I2C_RESPONSE_MALFORMED          = 4,
    I2C_RESPONSE_INCOMPLETE         = 5,
    I2C_RESPONSE_COMMAND_FAILED     = 6,
    I2C_RESPONSE_DEVICE_NOT_FOUND   = 7
}  I2CResponse;


// Main interface functions
void init_sensor_bus(I2CInterface *i2cInterface);
void shutdown_sensor_bus(I2CInterface *i2cInterface);
void reset_sensor_bus(I2CInterface *i2cInterface, bool fullReset);
I2CResponse check_i2c_address(I2CInterface *i2cInterface, const uint8_t address);
I2CResponse select_i2c_channel(I2CInterface *i2cInterface, I2CChannel channel);
bool is_i2c_channel_connected(I2CInterface *i2cInterface, I2CChannel channel);
void update_i2c_connection_status(I2CInterface *i2cInterface);
I2CResponse write_i2c_data(
    I2CInterface *i2cInterface, 
    const uint8_t address, 
    const uint8_t *buffer, 
    size_t bufferLen 
);
I2CResponse write_prefixed_i2c_data(
    I2CInterface *i2cInterface, 
    const uint8_t address, 
    const uint8_t *prefixBuffer, 
    size_t prefixLen,
    const uint8_t *buffer, 
    size_t bufferLen 
);
I2CResponse write_to_i2c_register(
    I2CInterface *i2cInterface, 
    const uint8_t address, 
    const uint8_t regHigh, 
    const uint8_t regLow, 
    const uint8_t *buffer, 
    const uint8_t bufferLen
);
I2CResponse read_from_i2c(
    I2CInterface *i2cInterface,
    const uint8_t address,
    uint8_t *buffer, 
    const uint8_t amountToRead
);
I2CResponse read_from_i2c_register(
    I2CInterface *i2cInterface,
    const uint8_t address,
    const uint8_t regHigh, 
    const uint8_t regLow,
    uint8_t *buffer, 
    const uint8_t amountToRead, 
    const uint16_t readDelay
);


#endif
