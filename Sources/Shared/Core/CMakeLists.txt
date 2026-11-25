cmake_minimum_required(VERSION 3.26)

set(SOURCES
    Math/EasingFunction.hpp
    Math/Projection.hpp
    Math/View.hpp
    Camera.cpp
    Camera.hpp
    EnumToString.hpp
    Log.cpp
    Log.hpp
    Path.hpp
    Thread.cpp
    Thread.hpp
)

add_library(Core STATIC ${SOURCES})

target_include_directories(
    Core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(Core
        MathLib
)
