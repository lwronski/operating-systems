cmake_minimum_required(VERSION 3.5.1)
project(cw1_zad1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC ")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

enable_testing()

foreach(loop_var -O -O1 -O2 -O3 -Os)

    set(sharedName "mySharedLibrary${loop_var}")
    set(staticName "myStaticLibrary${loop_var}")
    set(dynamicName "myDynamicLibrary${loop_var}")


    add_library(${staticName} STATIC blocks.c)
    add_library(${sharedName}  SHARED blocks.c)
    add_library(${dynamicName}  MODULE blocks.c)

    target_compile_options(${sharedName} PUBLIC ${loop_var})
    target_compile_options(${staticName} PUBLIC ${loop_var})
    target_compile_options(${dynamicName} PUBLIC ${loop_var})

    set(mainShared "mainShared${loop_var}")
    set(mainStatic "mainStatic${loop_var}")
    set(mainDynamic "mainDynamic${loop_var}")

    add_executable(${mainStatic} main.c)
    add_executable(${mainShared} main.c)
    add_executable(${mainDynamic} main_dyn.c)

    target_compile_options(${mainStatic} PUBLIC ${loop_var})
    target_compile_options(${mainShared} PUBLIC ${loop_var})
    target_compile_options(${mainDynamic} PUBLIC ${loop_var})

    target_link_libraries(${mainStatic} ${staticName})
    target_link_libraries(${mainShared} ${sharedName})
    target_link_libraries(${mainDynamic} ${CMAKE_DL_LIBS})


    add_test(NAME ${mainStatic}${loop_var} COMMAND ${mainStatic} create_table 10000 search_directory ~  "*"  large  save_temp_file_to_blocks large search_directory /usr   "*"  medium  save_temp_file_to_blocks medium search_directory ./  "*"  small  save_temp_file_to_blocks small  remove_block 0 remove_block 1 remove_block 2 search_directory_n_times /usr  "*"  medium 10  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    add_test(NAME ${mainDynamic}${loop_var} COMMAND ${mainDynamic} ./lib${dynamicName}.so create_table 10000 search_directory ~  "*"  large  save_temp_file_to_blocks large search_directory /usr   "*"  medium  save_temp_file_to_blocks medium search_directory ./  "*"  small  save_temp_file_to_blocks small  remove_block 0 remove_block 1 remove_block 2 search_directory_n_times /usr  "*"  medium 10 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    add_test(NAME ${mainShared}${loop_var} COMMAND ${mainShared} create_table 10000 search_directory ~  "*"  large  save_temp_file_to_blocks large search_directory /usr "*"  medium  save_temp_file_to_blocks medium search_directory ./  "*"  small  save_temp_file_to_blocks small  remove_block 0 remove_block 1 remove_block 2 search_directory_n_times /usr  "*"   medium  10 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})


endforeach(loop_var)
