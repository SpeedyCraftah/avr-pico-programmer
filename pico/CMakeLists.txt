cmake_minimum_required(VERSION 3.13)

include(/opt/pico-sdk/external/pico_sdk_import.cmake)
include(/opt/pico-extras/external/pico_extras_import.cmake)
 
project(prog C CXX ASM)
 
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
 
pico_sdk_init()

add_executable(${PROJECT_NAME} main.c avrprog.c)
pico_add_extra_outputs(${PROJECT_NAME})
 
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
