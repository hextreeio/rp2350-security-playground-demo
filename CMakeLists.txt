cmake_minimum_required(VERSION 3.13)
# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(1_simple_loop)

pico_sdk_init()

# First executable
add_executable(1_simple_loop)

target_sources(1_simple_loop PRIVATE
        main.c
        ../lib/rp2350_playground.c
        ../lib/display/display.c
        ../lib/display/font_renderer.c
        ../lib/display/FreeSans5pt7b.c
        ../lib/display/FreeSans9pt7b.c
        ../lib/display/ssd1306.c)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_link_libraries(1_simple_loop PRIVATE
        pico_stdlib
        hardware_pio
        hardware_i2c
        )

pico_add_extra_outputs(1_simple_loop)

# enable usb output, disable uart output
pico_enable_stdio_usb(1_simple_loop 1)
pico_enable_stdio_uart(1_simple_loop 0)

# Second executable with GLITCH_DETECTOR_ON defined
add_executable(1_simple_loop_glitch_detector)

target_sources(1_simple_loop_glitch_detector PRIVATE
        main.c
        ../lib/rp2350_playground.c
        ../lib/display/display.c
        ../lib/display/font_renderer.c
        ../lib/display/FreeSans5pt7b.c
        ../lib/display/FreeSans9pt7b.c
        ../lib/display/ssd1306.c)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib")

target_compile_definitions(1_simple_loop_glitch_detector PRIVATE GLITCH_DETECTOR_ON)

target_link_libraries(1_simple_loop_glitch_detector PRIVATE
        pico_stdlib
        hardware_pio
        hardware_i2c
        )

pico_add_extra_outputs(1_simple_loop_glitch_detector)

# enable usb output, disable uart output
pico_enable_stdio_usb(1_simple_loop_glitch_detector 1)
pico_enable_stdio_uart(1_simple_loop_glitch_detector 0)