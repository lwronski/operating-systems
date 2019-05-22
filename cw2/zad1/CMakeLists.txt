cmake_minimum_required(VERSION 3.13)
project(cw2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(main main.c)

enable_testing()

add_test(NAME test COMMAND main -generate test 100 100 -generate test2 100 100 -sort test 100 100 lib -sort test2 100 100 sys -copy test test3 100 100 lib -copy test2 test4 100 100 sys WORKING_DIRECTORY ${CMAKE_BINARY_DIR})