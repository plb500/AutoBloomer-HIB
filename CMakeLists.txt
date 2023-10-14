cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(PiFeederSensors C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(PiFeederSensors
    src/sensor_definitions.c
    
    src/mpack/mpack.c
    
    src/hardware/sensors/scd30_sensor.c
    src/hardware/sensors/sensor_i2c_interface.c
    src/hardware/sensors/sensor_pod.c
    src/hardware/sensors/sensor.c
    src/hardware/sensors/sonar_sensor.c
    src/hardware/sensors/stemma_soil_sensor.c
    src/hardware/shift_register.c

    src/uart_controller/sensor_msgpack.c
    src/uart_controller/uart_sensor_controller.c

    src/sensor_multicore/sensor_hardware_core_0.c
    src/sensor_multicore/sensor_uart_control_core_1.c
    src/sensor_multicore/sensor_multicore_utils.c
)

pico_generate_pio_header(PiFeederSensors ${CMAKE_CURRENT_LIST_DIR}/src/pio/uart_rx.pio)

pico_enable_stdio_usb(PiFeederSensors 0)
pico_enable_stdio_uart(PiFeederSensors 1)
pico_add_extra_outputs(PiFeederSensors)
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(PiFeederSensors
    pico_stdlib
    hardware_i2c
    hardware_uart
    hardware_pio
    pico_util
    pico_multicore 
    hardware_flash
)
