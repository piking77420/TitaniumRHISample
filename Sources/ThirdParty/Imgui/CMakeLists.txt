cmake_minimum_required(VERSION 3.26)

set (IMGUI_DIR ${THIRD_PARTY_DIR}/Imgui) 

set(SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

if (TITANIUM_RHI_BACKEND STREQUAL Vulkan)
list(APPEND SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)
elseif (TITANIUM_RHI_BACKEND STREQUAL DirectX12)
list(APPEND SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_dx12.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)
elseif (TITANIUM_RHI_BACKEND STREQUAL Metal)
else()

endif()


add_library(Imgui STATIC ${SOURCES})

target_link_libraries(Imgui
        glfw
)

if (TITANIUM_RHI_BACKEND STREQUAL Vulkan)
    find_package(Vulkan REQUIRED)
    find_package(Vulkan REQUIRED COMPONENTS volk)

    target_link_libraries(Imgui
        Vulkan::Headers
        Vulkan::Vulkan
        Vulkan::volk
    )

    target_compile_definitions(Imgui PRIVATE
        IMGUI_IMPL_VULKAN_USE_VOLK
        "IMGUI_IMPL_VULKAN_VOLK_FILENAME=<Volk/volk.h>"
    )
elseif (TITANIUM_RHI_BACKEND STREQUAL DirectX12)
target_link_libraries(Imgui
        d3d12.lib 
        dxgi.lib 
        dxguid.lib
)
elseif (TITANIUM_RHI_BACKEND STREQUAL Metal)
else()

endif()

target_include_directories(Imgui PUBLIC
    "${THIRD_PARTY_DIR}/Imgui"
    "${THIRD_PARTY_DIR}/Imgui/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
