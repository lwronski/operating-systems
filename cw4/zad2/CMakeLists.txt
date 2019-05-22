cmake_minimum_required(VERSION 3.3)
project(cw3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_library(sharedLibraryFile SHARED file_service.c)
add_library(sharedLibraryMonitor SHARED monitor.c)

add_executable(monitor main.c)


target_link_libraries(monitor sharedLibraryFile)
target_link_libraries(monitor sharedLibraryMonitor)
