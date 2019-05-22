cmake_minimum_required(VERSION 2.13)
project(cw7 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_library(staticLibraryCommon STATIC common.c)
add_library(staticLibraryBelt STATIC belt.c)

add_executable(trucker trucker.c)
add_executable(loader loader.c)
add_executable(main main.c)

target_link_libraries(loader  staticLibraryCommon)
target_link_libraries(trucker  staticLibraryCommon)
target_link_libraries(main  staticLibraryCommon)

target_link_libraries(loader  staticLibraryBelt)
target_link_libraries(trucker  staticLibraryBelt)
