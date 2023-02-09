cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(PiFeederSensors C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(PiFeederSensors
    src/pi_feeder_sensor_core.c
    src/sensor_definitions.c
    src/mpack/mpack.c
    src/hardware/push_button.c
    src/hardware/led_shifter.c
    src/hardware/sensors/sensor.c
    src/hardware/sensors/dht_sensor.c
    src/hardware/sensors/stemma_soil_sensor.c
    src/hardware/sensors/sonar_sensor.c
    src/hardware/sensors/sensor_msgpack.c
    src/sensor_controller/sensor_controller.c
    src/sensor_multicore/sensor_multicore_utils.c
    src/sensor_multicore/sensor_controller_core.c
)

pico_generate_pio_header(PiFeederSensors ${CMAKE_CURRENT_LIST_DIR}/src/pio/uart_rx.pio)

pico_enable_stdio_usb(PiFeederSensors 0)
pico_enable_stdio_uart(PiFeederSensors 1)
pico_add_extra_outputs(PiFeederSensors)
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/extern/hx711-pico-c
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
