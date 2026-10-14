cmake_minimum_required(VERSION 3.26)

set(SOURCES
    Thread.cpp
    Thread.hpp
)

add_library(Core STATIC ${SOURCES})

target_include_directories(
    Core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
