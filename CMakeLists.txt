# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# needed for building lua without warnings
cmake_policy(SET CMP0076 NEW)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
include(pico_extras_import.cmake)


# Set name of project (as PROJECT_NAME) and C/C++ standards
project(miditocv C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# error if there is a warning
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall")

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
    main.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/hw_config.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/pcg_basic.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/libmidi.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/usb_descriptors.c
)


# Tell CMake where to find other source code
add_subdirectory(lib/sdio build)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lua-5.4.6/src)

# generate pio
pico_generate_pio_header(miditocv ${CMAKE_CURRENT_LIST_DIR}/lib/WS2812.pio)
pico_generate_pio_header(miditocv ${CMAKE_CURRENT_LIST_DIR}/lib/uart_rx.pio)
# pico_generate_pio_header(miditocv ${CMAKE_CURRENT_LIST_DIR}/lib/buttonmatrix3.pio)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_multicore
    FatFs_SPI
    hardware_clocks
    hardware_flash
    hardware_adc
    hardware_pio
    hardware_i2c
    hardware_sync
    hardware_spi
    lua
)

# target_compile_options(command_line PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)
# target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wno-unused-parameter)

# This program is useless without standard standard input and output.
# add_compile_definitions(USE_PRINTF USE_DBG_PRINTF)
add_compile_definitions(USE_PRINTF)

set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "-Wl,--print-memory-usage")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    # define board
    INCLUDE_YOCTOCORE=1

    # hardware specific
    SDCARD_USE_CD=false
    SDCARD_CD_GPIO=21 # not used...
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=4
    # DO_OVERCLOCK=1

    # pins
    SDCARD_CMD_GPIO=11
    SDCARD_D0_GPIO=12
    I2C0_SDA_PIN=4
    I2C0_SCL_PIN=5
    I2C1_SDA_PIN=2
    I2C1_SCL_PIN=3
    PIN_SPI_CSN=17
    PIN_SPI_CLK=18
    PIN_SPI_RX=16
    PIN_SPI_TX=19
    MIDI_RX_PIN=9
    WS2812_PIN=7
    WS2812_SM=2
    WS2812_NUM_LEDS=16

    # constants
    REFERENCE_5V=5.0
    MAX_NOTE_HOLD_TIME_MS=30000
    # # settings
    INCLUDE_MIDI=1

    # debugging
    # DEBUG_MIDI=1
    # DEBUG_VOLTAGE_CALIBRATION=1

    USBD_PID=0x1836
)

# if INCLUDE_MIDI
target_link_libraries(${PROJECT_NAME}
    tinyusb_device
    tinyusb_board
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    pico_enable_stdio_usb(${PROJECT_NAME} 0)
    add_compile_definitions(PICO_STDIO_USB_ENABLE=0)
else()
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    add_compile_definitions(PICO_STDIO_USB_ENABLE=1)
endif()
pico_enable_stdio_uart(${PROJECT_NAME} 1)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# # if NOT INCLUDE_MIDI
# pico_enable_stdio_usb(${PROJECT_NAME} 1)
# pico_enable_stdio_uart(${PROJECT_NAME} 1)


pico_add_extra_outputs(${PROJECT_NAME})
