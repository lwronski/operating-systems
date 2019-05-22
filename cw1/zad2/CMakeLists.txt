cmake_minimum_required(VERSION 3.5.1)
project(cw1_zad1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_library(staticLibrary STATIC blocks.c)
add_library(sharedLibrary SHARED blocks.c)

add_executable(mainStatic main.c)
add_executable(mainShared main.c)

target_link_libraries(mainStatic staticLibrary)
target_link_libraries(mainShared sharedLibrary)