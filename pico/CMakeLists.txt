cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(AutoBloomer-HIB C CXX ASM)

pico_sdk_init()

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")

file(READ wifi_props/ssid SSID)
file(READ wifi_props/key SSID_KEY)
file(READ wifi_props/broker BROKER)
set(PICO_HOSTNAME "SensorController" CACHE INTERNAL "Pico hostname")

add_executable(AutoBloomer-HIB
    pico_src/sensor_definitions.c
    
    pico_src/hardware/sensors/battery_sensor.c
    pico_src/hardware/sensors/scd30_sensor.c
    pico_src/hardware/sensors/sensor_i2c_interface.c
    pico_src/hardware/sensors/sensor_pod.c
    pico_src/hardware/sensors/sensor.c
    pico_src/hardware/sensors/sonar_sensor.c
    pico_src/hardware/sensors/stemma_soil_sensor.c
    pico_src/hardware/shift_register.c
    pico_src/hardware/connected_hardware_monitor.c

    pico_src/network/network_utils.c
    pico_src/network/mqtt_utils.c

    pico_src/sensor_multicore/sensor_hardware_core_0.c
    pico_src/sensor_multicore/sensor_mqtt_client_core_1.c
    pico_src/sensor_multicore/sensor_multicore_utils.c
)

pico_generate_pio_header(AutoBloomer-HIB ${CMAKE_CURRENT_LIST_DIR}/pico_src/pio/uart_rx.pio)

pico_enable_stdio_usb(AutoBloomer-HIB 0)
pico_enable_stdio_uart(AutoBloomer-HIB 1)
pico_add_extra_outputs(AutoBloomer-HIB)

target_compile_definitions(AutoBloomer-HIB PRIVATE
    WIFI_SSID=\"${SSID}\"
    WIFI_PASSWORD=\"${SSID_KEY}\"
    MQTT_BROKER=\"${BROKER}\"
    PICO_HOSTNAME=\"${PICO_HOSTNAME}\"
)

target_include_directories(AutoBloomer-HIB PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
    ${PROJECT_SOURCE_DIR}/pico_src
    ${PROJECT_SOURCE_DIR}/pico_src/network
)

target_link_libraries(AutoBloomer-HIB
    pico_stdlib
    hardware_adc
    hardware_i2c
    hardware_uart
    hardware_pio
    pico_util
    pico_multicore 
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
)
