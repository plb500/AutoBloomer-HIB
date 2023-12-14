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
    SONAR_SENSOR_L1_ACTIVE_LED      = LED_L1,
    SONAR_SENSOR_R1_ACTIVE_LED      = LED_R1,
    SENSOR_POD_L_ACTIVE_LED         = LED_L2,
    SENSOR_POD_R_ACTIVE_LED         = LED_R2,
    NO_LED                          = -1
} SensorActiveLEDPositions;

// I2C sensors values
#define SENSOR_I2C                                      (i2c1)
static const uint SENSOR_I2C_BAUDRATE                   = (40 * 1000);
static const uint8_t SENSOR_I2C_SDA                     = 10;
static const uint8_t SENSOR_I2C_SCL                     = 11;

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
