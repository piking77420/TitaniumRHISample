cmake_minimum_required(VERSION 3.26)

set(SOURCES
    GpuResource.cpp
    GpuResource.hpp
    RenderThread.cpp
    RenderThread.hpp
    RhiThread.cpp
    RhiThread.hpp
    StaticMesh.cpp
    StaticMesh.hpp
    Texture.cpp
    Texture.hpp
)

add_library(Rendering STATIC ${SOURCES})

target_include_directories(
    Rendering
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(Rendering
    PUBLIC
        TitaniumRHI
        MathLib
    PRIVATE
        Core
)