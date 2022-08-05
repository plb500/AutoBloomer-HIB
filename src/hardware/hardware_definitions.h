#ifndef HARDWARE_DEFINITIONS_H
#define HARDWARE_DEFINITIONS_H

#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"

// Time utility
#define MILLIS() (to_ms_since_boot(get_absolute_time()))

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

// Debug/Serial output macro block
#define DEBUG_PRINT_ON                                  1       // Set to 0 to disable all stdio UART logging

#if DEBUG_PRINT_ON
#   define DEBUG_PRINT_BUF_SIZE                         (128)
static char DEBUG_PRINT_BUF[DEBUG_PRINT_BUF_SIZE];

// Debug logging pins and values (UART1)
#   define STDIO_UART                                   (uart0)
static const uint8_t UART_RX                            = 8;
static const uint8_t UART_TX                            = 9;
static const int STDIO_UART_BAUDRATE                    = 19200;


// Initialize the debug logging system
#   define DEBUG_PRINT_INIT()                              { \
        gpio_set_function(UART_TX, GPIO_FUNC_UART); \
        gpio_set_function(UART_RX, GPIO_FUNC_UART); \
        uart_init(STDIO_UART, STDIO_UART_BAUDRATE); \
        uart_set_format (STDIO_UART, 8, 1, UART_PARITY_NONE); \
        uart_set_hw_flow(STDIO_UART, false, false); \
    }

// Print the supplied sformatted string to debug out UART
#   define DEBUG_PRINT(format, ...)                        { \
        snprintf(DEBUG_PRINT_BUF, DEBUG_PRINT_BUF_SIZE, format __VA_OPT__(,) __VA_ARGS__); \
        uart_puts(STDIO_UART, DEBUG_PRINT_BUF); \
    }

// Shut down the debug logging UART
#   define DEBUG_PRINT_DEINIT()                            { \
    (uart_deinit(STDIO_UART); \
    }
#else
// No-op all debug logging functions
#   define DEBUG_PRINT_INIT()                              {}
#   define DEBUG_PRINT(format, ...)                        {}
#   define DEBUG_PRINT_DEINIT()                            {}
#endif

#endif  // HARDWARE_DEFINITIONS_H
