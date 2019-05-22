cmake_minimum_required(VERSION 3.13)
project(cw3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall  -fPIC")


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_library(staticLibraryFile STATIC file_service.c)
add_library(staticLibrarySignalHelper STATIC signal_helper.c)

add_executable(sender sender.c)
add_executable(catcher catcher.c)

target_link_libraries(sender staticLibraryFile)
target_link_libraries(sender staticLibrarySignalHelper)
target_link_libraries(catcher staticLibraryFile)
target_link_libraries(catcher staticLibrarySignalHelper)
