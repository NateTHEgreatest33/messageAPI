cmake_minimum_required(VERSION 3.13)
project(messageAPI C ASM)

set(CMAKE_C_STANDARD 11)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/loRa )

pico_sdk_init()

add_library( messageAPI
    messageAPI.hpp
    messageAPI.cpp
    )

target_link_libraries( messageAPI
  pico_stdlib 
  hardware_spi
  # Using Pico W
  pico_cyw43_arch_none

  # libraries
  loraAPI
  consoleAPI
)

target_include_directories(messageAPI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")