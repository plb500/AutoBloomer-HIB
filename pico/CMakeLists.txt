cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(PiFeederSensors C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")

add_executable(PiFeederSensors
    pico_src/sensor_definitions.c
    
    pico_src/mpack/mpack.c
    
    pico_src/hardware/sensors/battery_sensor.c
    pico_src/hardware/sensors/scd30_sensor.c
    pico_src/hardware/sensors/sensor_i2c_interface.c
    pico_src/hardware/sensors/sensor_pod.c
    pico_src/hardware/sensors/sensor.c
    pico_src/hardware/sensors/sonar_sensor.c
    pico_src/hardware/sensors/stemma_soil_sensor.c
    pico_src/hardware/shift_register.c
    pico_src/hardware/connected_hardware_monitor.c

    pico_src/uart_controller/sensor_msgpack.c
    pico_src/uart_controller/uart_sensor_controller.c

    pico_src/sensor_multicore/sensor_hardware_core_0.c
    pico_src/sensor_multicore/sensor_uart_control_core_1.c
    pico_src/sensor_multicore/sensor_multicore_utils.c
)

pico_generate_pio_header(PiFeederSensors ${CMAKE_CURRENT_LIST_DIR}/pico_src/pio/uart_rx.pio)

pico_enable_stdio_usb(PiFeederSensors 0)
pico_enable_stdio_uart(PiFeederSensors 1)
pico_add_extra_outputs(PiFeederSensors)
include_directories(
    ${PROJECT_SOURCE_DIR}/pico_src
)
target_link_libraries(PiFeederSensors
    pico_stdlib
    hardware_adc
    hardware_i2c
    hardware_uart
    hardware_pio
    pico_util
    pico_multicore 
    hardware_flash
)
