cmake_minimum_required(VERSION 2.1)
project(cw6 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_library(staticLibraryCommon STATIC common.c)

add_executable(client client.c)
add_executable(server server.c)

target_link_libraries(client  staticLibraryCommon)
target_link_libraries(server staticLibraryCommon)
