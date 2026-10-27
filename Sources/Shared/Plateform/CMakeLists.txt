cmake_minimum_required(VERSION 3.26)

set(SOURCES
    App.cpp
    App.hpp
    Window.cpp
    Window.hpp
)

add_library(Plateform STATIC ${SOURCES})

target_include_directories(
    Plateform
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(Plateform
    PRIVATE
        glfw
        TitaniumRHI

)