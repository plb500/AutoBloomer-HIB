#ifndef _HARDWARE_DEFINITIONS_H_
#define _HARDWARE_DEFINITIONS_H_

#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

// Sensor status LED shift register positions
#define LED_L1      (0)
#define LED_L2      (1)
#define LED_L3      (2)
#define LED_L4      (3)
#define LED_R1      (4)
#define LED_R2      (5)
#define LED_R3      (6)
#define LED_R4      (7)

typedef enum {
    SONAR_SENSOR_L1_ACTIVE_LED      = 0,
    SONAR_SENSOR_R1_ACTIVE_LED      = 4,
    SENSOR_POD_L_ACTIVE_LED         = 1,
    SENSOR_POD_R_ACTIVE_LED         = 5,
    LED_L3_IDX                      = 2,
    LED_L4_IDX                      = 3,
    LED_R3_IDX                      = 6,
    LED_R4_IDX                      = 7,
    NO_LED                          = -1
} SensorActiveLEDPositions;

typedef enum {
    SONAR_SENSOR_L1_ACTIVE_LED_IDX      = LED_L1,
    SONAR_SENSOR_R1_ACTIVE_LED_IDX      = LED_R1,
    SENSOR_POD_L_ACTIVE_LED_IDX         = LED_L2,
    SENSOR_POD_R_ACTIVE_LED_IDX         = LED_R2,
    NO_LED_IDX                          = -1
} SensorActiveLEDIndices;

typedef enum {
    BATTERY_SENSE_ENABLE_PIN                = 0,
    BATTERY_SENSE_PIN                       = 26,

    HARDWARE_CONNECT_SR_LATCH_PIN           = 20,
    HARDWARE_CONNECT_SR_CLOCK_PIN           = 14,
    HARDWARE_CONNECT_SR_DATA_PIN            = 15,

    CONNECTION_INDICATOR_LED_SR_LATCH_PIN   = 17,
    CONNECTION_INDICATOR_LED_SR_CLOCK_PIN   = 16,
    CONNECTION_INDICATOR_LED_SR_DATA_PIN    = 18,

    SENSOR_I2C_SDA_PIN                      = 10,
    SENSOR_I2C_SCL_PIN                      = 11,
    SENSOR_I2C_MULTIPLEXER_RESET_PIN        = 1,

    SONAR_SENSOR_L1_RX_PIN                  = 4,
    SONAR_SENSOR_L1_TX_PIN                  = 5,
    SONAR_SENSOR_L2_RX_PIN                  = 6,
    SONAR_SENSOR_L2_TX_PIN                  = 7,
    SONAR_SENSOR_R1_RX_PIN                  = 27,
    SONAR_SENSOR_R1_TX_PIN                  = 28,
    SONAR_SENSOR_R2_RX_PIN                  = 21,
    SONAR_SENSOR_R2_TX_PIN                  = 22,

    SENSOR_CONTROLLER_TX_PIN                = 8,
    SENSOR_CONTROLLER_RX_PIN                = 9,

    DEBUG_UART_TX_PIN                       = 12,
    DEBUG_UART_RX_PIN                       = 13,

    LED_SR_LATCH_PIN                        = 17,
    LED_SR_CLOCK_PIN                        = 16,
    LED_SR_DATA_PIN                         = 18
} PinLabel;

typedef enum {
    FEED_SENSOR_L1_CONNECT_ID               = 0,
    FEED_SENSOR_L2_CONNECT_ID               = 1,
    FEED_SENSOR_R1_CONNECT_ID               = 3,
    FEED_SENSOR_R2_CONNECT_ID               = 2,

    I2C_DEVICE_0_CONNECT_ID                 = 8,
    I2C_DEVICE_1_CONNECT_ID                 = 9,
    I2C_DEVICE_2_CONNECT_ID                 = 10,
    I2C_DEVICE_3_CONNECT_ID                 = 11,
    I2C_DEVICE_4_CONNECT_ID                 = 12,
    I2C_DEVICE_5_CONNECT_ID                 = 13,
    I2C_DEVICE_6_CONNECT_ID                 = 14,
    I2C_DEVICE_7_CONNECT_ID                 = 15,
    
    ALWAYS_CONNECTED_CONNECT_ID             = -1,
    NEVER_CONNECTED_CONNECT_ID              = -2
} HardwareConnectID;


// I2C sensors values
#define SENSOR_I2C                                      (i2c1)
static const uint SENSOR_I2C_BAUDRATE                   = (10 * 1000);
static const uint8_t SENSOR_I2C_SDA                     = 10;
static const uint8_t SENSOR_I2C_SCL                     = 11;
static const uint8_t SENSOR_I2C_MULTIPLEXER_RESET       = 19;

// Sonar (feed level) sensor data pins (outputs)
static const uint8_t SONAR_SENSOR_RX_L1                  = 4;
static const uint8_t SONAR_SENSOR_TX_L1                  = 5;
static const uint8_t SONAR_SENSOR_L1_JACK_DETECT_PIN     = 0;

static const uint8_t SONAR_SENSOR_RX_L2                  = 6;
static const uint8_t SONAR_SENSOR_TX_L2                  = 7;
static const uint8_t SONAR_SENSOR_L2_JACK_DETECT_PIN     = 1;

static const uint8_t SONAR_SENSOR_RX_R1                  = 27;
static const uint8_t SONAR_SENSOR_TX_R1                  = 28;
static const uint8_t SONAR_SENSOR_R1_JACK_DETECT_PIN     = 2;

static const uint8_t SONAR_SENSOR_RX_R2                  = 21;
static const uint8_t SONAR_SENSOR_TX_R2                  = 22;
static const uint8_t SONAR_SENSOR_R2_JACK_DETECT_PIN     = 3;

static const int SONAR_SENSOR_BAUDRATE                  = 9600;
#define SONAR_SENSOR_PIO                                (pio0)

// Main controller comms pins and values (UART1)
#define SENSOR_CONTROLLER_UART                          (uart1)
static const uint8_t SENSOR_CONTROLLER_TX               = 8;
static const uint8_t SENSOR_CONTROLLER_RX               = 9;
static const int SENSOR_CONTROLLER_BAUDRATE             = 57600;

// Debug logging pins and values (UART0)
#define STDIO_UART                                      (uart0)
static const uint8_t UART_TX                            = 12;
static const uint8_t UART_RX                            = 13;
static const int STDIO_UART_BAUDRATE                    = 57600;

// I/O Shift register pins
static const uint8_t PISO_LATCH_PIN                     = 20;
static const uint8_t PISO_CLOCK_PIN                     = 14;
static const uint8_t PISO_DATA_PIN                      = 15;

static const uint8_t SIPO_LATCH_PIN                     = 17;
static const uint8_t SIPO_CLOCK_PIN                     = 16;       
static const uint8_t SIPO_DATA_PIN                      = 18;

// RTC Battery level pin
static const uint8_t RTC_BATTERY_SENSE_PIN              = 26;
static const uint8_t RTC_BATTERY_ADC_PORT               = 0;

#endif  // HARDWARE_DEFINITIONS_H
