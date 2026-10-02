cmake_minimum_required(VERSION 3.26)

project(Renderer VERSION 0.1.0 LANGUAGES CXX)

set(RENDERER_SOURCES
    main.cpp
    Window.cpp
    Window.hpp
)

add_executable(
    ${PROJECT_NAME}
    ${RENDERER_SOURCES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        TitaniumRHI
)