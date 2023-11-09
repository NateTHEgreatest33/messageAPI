cmake_minimum_required(VERSION 3.13)

# include(pico_sdk_import.cmake)
project(messageAPI C ASM)

set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 20)
add_subdirectory( LoRa )
pico_sdk_init()

add_library( messageAPI
    messageAPI.h
    messageAPI.c
    )

target_link_libraries( messageAPI
  pico_stdlib 
  hardware_spi
  # Using Pico W
  pico_cyw43_arch_none

  loraAPI
)

target_include_directories(messageAPI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")