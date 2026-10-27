cmake_minimum_required(VERSION 3.26)

set(SOURCES
    File.cpp
    File.hpp
    Io.cpp
    Io.hpp
)

add_library(Io STATIC ${SOURCES})

target_include_directories(
    Io
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(Io
    PRIVATE
        glfw
)