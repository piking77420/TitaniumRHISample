cmake_minimum_required(VERSION 3.26)


project(Renderer VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(Shared/Core)
add_subdirectory(Shared/Io)
add_subdirectory(Shared/Plateform)

set(RENDERER_SOURCES
    main.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${RENDERER_SOURCES}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    TITANIUM_RESOURCE_DIR="${CMAKE_SOURCE_DIR}/Resources"
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
        TitaniumShaderCompiler
        Core
        Io
        Plateform
)
if(TITANIUM_USE_PIX)
    message(STATUS "Copy PIX DLL: ${TITANIUM_PIX_DLL}")

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TITANIUM_PIX_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
    )
endif()