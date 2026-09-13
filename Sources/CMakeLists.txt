cmake_minimum_required(VERSION 3.26)

project(Renderer VERSION 0.1.0 LANGUAGES CXX)


set(RENDERER_SOURCES
    main.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${RENDERER_SOURCES}
)