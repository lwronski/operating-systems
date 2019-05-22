cmake_minimum_required(VERSION 3.13)
project(cw5 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_library(staticLibraryFileHelper STATIC file_service.c)

add_executable(master master.c)
add_executable(slave slave.c)

target_link_libraries(slave staticLibraryFileHelper)
target_link_libraries(master staticLibraryFileHelper)