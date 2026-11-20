cmake_minimum_required(VERSION 3.26)

set(SOURCES
    ImGuiIntegration.cpp
    ImGuiIntegration.hpp
)

add_library(ImGuiIntegration STATIC ${SOURCES})

target_include_directories(Imgui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(ImGuiIntegration Imgui TitaniumRHI Plateform)