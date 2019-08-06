cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/bzip2)

aux_source_directory(
        ${CMAKE_SOURCE_DIR}/bzip2/ bzip2_src
)

aux_source_directory(
        ${CMAKE_SOURCE_DIR}/bspath/ bspath_src
)

add_library(
        native-lib
        SHARED
        native-lib.cpp
        ${bspath_src}
        ${bzip2_src}
)


find_library(
        log-lib
        log)


target_link_libraries(
        native-lib
        ${log-lib})