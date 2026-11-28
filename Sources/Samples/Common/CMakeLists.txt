cmake_minimum_required(VERSION 3.26)

set(SOURCES
    CameraController.cpp
    CameraController.hpp
    CameraExemple.cpp
    CameraExemple.hpp
    Exemple.cpp
    Exemple.hpp
)

add_library(Common STATIC ${SOURCES})

target_include_directories(
    Common
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
