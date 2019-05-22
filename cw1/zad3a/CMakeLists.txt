cmake_minimum_required(VERSION 3.5)
project(cw1_zad1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC ")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_library(myStaticLibrary STATIC blocks.c)
add_library(mySharedLibrary SHARED blocks.c)
add_library(myDynamicLibrary MODULE blocks.c)

add_executable(mainStatic main.c)
add_executable(mainShared main.c)
add_executable(mainDynamic main_dyn.c)

target_link_libraries(mainStatic myStaticLibrary)
target_link_libraries(mainShared mySharedLibrary)
target_link_libraries(mainDynamic ${CMAKE_DL_LIBS})

enable_testing()


add_test(NAME testStatic1 COMMAND mainDynamic ./libmyDynamicLibrary.so create_table 10000 search_directory ~  "*"  large  save_temp_file_to_blocks large search_directory /usr   "*"  medium  save_temp_file_to_blocks medium search_directory ./  "*"  small  save_temp_file_to_blocks small  remove_block 0 remove_block 1 remove_block 2 search_directory_n_times ~  "*"  medium 10  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME testStatic2 COMMAND mainStatic create_table 10000 search_directory ~ "*"  large  save_temp_file_to_blocks large search_directory /usr   "*"  medium  save_temp_file_to_blocks medium search_directory ./  "*"  small  save_temp_file_to_blocks small  remove_block 0 remove_block 1 remove_block 2 search_directory_n_times ~  "*"  medium 10 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME testStatic3 COMMAND mainShared  create_table 10000 search_directory ~  "*"  large  save_temp_file_to_blocks large search_directory /usr  "*"  medium  save_temp_file_to_blocks medium search_directory ./  "*"  small  save_temp_file_to_blocks small  remove_block 0 remove_block 1 remove_block 2 search_directory_n_times ~  "*"   medium  10 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})