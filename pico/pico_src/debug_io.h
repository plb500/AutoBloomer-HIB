#ifndef DEBUG_IO_H
#define DEBUG_IO_H

#include "hardware_definitions.h"

// Debug/Serial output macro block
#define DEBUG_PRINT_ON                                  1       // Set to 0 to disable all stdio UART logging

#if DEBUG_PRINT_ON
#include <stdio.h>
#   define DEBUG_PRINT_BUF_SIZE                         (128)
static char DEBUG_PRINT_BUF[DEBUG_PRINT_BUF_SIZE];

// Initialize the debug logging system
#   define DEBUG_PRINT_INIT()                              { \
        gpio_set_function(DEBUG_UART_TX_PIN, GPIO_FUNC_UART); \
        gpio_set_function(DEBUG_UART_RX_PIN, GPIO_FUNC_UART); \
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


#endif
