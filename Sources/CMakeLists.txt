cmake_minimum_required(VERSION 3.26)


project(Renderer VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(Shared/Core)
add_subdirectory(Shared/Io)
add_subdirectory(Shared/Plateform)

set(PRIVATE_DEPENDENCIES
    glfw
    TitaniumRHI
    TitaniumShaderCompiler
    Core
    Io
    Plateform
)


set(TITANIUM_RESOURCE_DIR "${CMAKE_SOURCE_DIR}/Resources")
set(EXECUTABLE_NAMES "")
add_subdirectory(Samples/Triangle)


foreach(target IN LISTS EXECUTABLE_NAMES)
    target_compile_definitions(${target} PRIVATE
        TITANIUM_RESOURCE_DIR=\"${TITANIUM_RESOURCE_DIR}\"
    )

    target_include_directories(
        ${target}
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/Shared"
    )

    target_link_libraries(${target}
        ${PRIVATE_DEPENDENCIES}
    )

    if(TITANIUM_USE_PIX)
        message(STATUS "Copy PIX DLL: ${TITANIUM_PIX_DLL}")

        add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${TITANIUM_PIX_DLL}"
                "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
    endif()
endforeach()