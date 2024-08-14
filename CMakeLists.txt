cmake_minimum_required(VERSION 3.13)
# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(rp2350_security_demo)

pico_sdk_init()

add_executable(rp2350_security_demo)

target_sources(rp2350_security_demo PRIVATE
        main.c
        ../lib/rp2350_playground.c
        ../lib/display/display.c
        ../lib/display/font_renderer.c
        ../lib/display/FreeSans5pt7b.c
        ../lib/display/FreeSans9pt7b.c
        ../lib/display/ssd1306.c)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_compile_definitions(rp2350_security_demo PRIVATE GLITCH_DETECTOR_ON)

target_link_libraries(rp2350_security_demo PRIVATE
        pico_stdlib
        hardware_pio
        hardware_i2c
        hardware_powman
        )

pico_add_extra_outputs(rp2350_security_demo)

# enable usb output, disable uart output
pico_enable_stdio_usb(rp2350_security_demo 1)
pico_enable_stdio_uart(rp2350_security_demo 0)