#ifndef HARDWARE_DEFINITIONS_H
#define HARDWARE_DEFINITIONS_H

#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"

// Sensor status LED pins
typedef enum {
    LOAD_SENSOR_L_ACTIVE_LED        = 21,
    LOAD_SENSOR_R_ACTIVE_LED        = 16,
    TEMP_SENSOR_L_ACTIVE_LED        = 19,
    TEMP_SENSOR_R_ACTIVE_LED        = 18,
    MOISTURE_SENSOR_L_ACTIVE_LED    = 20,
    MOISTURE_SENSOR_R_ACTIVE_LED    = 17
} SensorActiveLEDPins;

// Sensor connection detection pins
typedef enum {
    LOAD_SENSOR_L_JACK_DETECT_PIN       = 1,
    LOAD_SENSOR_R_JACK_DETECT_PIN       = 4,
    TEMP_SENSOR_L_JACK_DETECT_PIN       = 2,
    TEMP_SENSOR_R_JACK_DETECT_PIN       = 3,
    MOISTURE_SENSOR_L_JACK_DETECT_PIN   = 0,
    MOISTURE_SENSOR_R_JACK_DETECT_PIN   = 5
} SensorActiveJackDetectPins;


// Soil moisture sensor pins (I2C-1)
#define SENSOR_I2C                                      (i2c1)
static const uint SENSOR_I2C_BAUDRATE                   = (100 * 1000);
static const uint8_t MOISTURE_I2C_SDA                   = 6;
static const uint8_t MOISTURE_I2C_SCL                   = 7;

// Load sensor data pins (outputs)
static const uint8_t LOAD_SENSOR_SDA_L                  = 28;
static const uint8_t LOAD_SENSOR_SCL_L                  = 27;
static const uint8_t LOAD_SENSOR_SDA_R                  = 26;
static const uint8_t LOAD_SENSOR_SCL_R                  = 22;

// Load sensor control pins (input)
static const uint8_t LOAD_SENSOR_TARE_L                 = 10;
static const uint8_t LOAD_SENSOR_TARE_R                 = 11;

// Temperature sensor pins (input/output)
static const uint8_t TEMP_SENSOR_L_CTL                  = 14;
static const uint8_t TEMP_SENSOR_R_CTL                  = 15;

// Main controller comms pins and values (UART0)
#define SENSOR_CONTROLLER_UART                          (uart1)
static const uint8_t SENSOR_CONTROLLER_RX               = 13;
static const uint8_t SENSOR_CONTROLLER_TX               = 12;
static const int SENSOR_CONTROLLER_BAUDRATE             = 19200;


#endif  // HARDWARE_DEFINITIONS_H
