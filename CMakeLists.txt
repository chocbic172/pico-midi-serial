cmake_minimum_required(VERSION 3.13)

# Setup for the RPI Pico Board
set(BOARD pico_sdk)
include(libraries/pico-sdk/pico_sdk_init.cmake)


# Create project and set name and version
set(target_proj PicoMIDI)
project(${target_proj} VERSION 1.0.0)


# Add source files to project
add_executable(${target_proj} source.c usb_descriptors.c)


# Make sure TinyUSB can find tusb_config.h
target_include_directories(${target_proj} PRIVATE ${CMAKE_CURRENT_LIST_DIR})


# Link required libraries
target_link_libraries(${target_proj} pico_stdlib pico_unique_id tinyusb_device tinyusb_board)


# Initalise the Pico SDK + add debug files (map, bin, hex)
pico_sdk_init()
pico_add_extra_outputs(${target_proj})