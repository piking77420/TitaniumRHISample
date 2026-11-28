cmake_minimum_required(VERSION 3.26)

project(TitianiumRHISample VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/ThirdParty")

set(VALIDATION_LAYER True)
if(CMAKE_GENERATOR STREQUAL "Xcode" OR APPLE)
    set(TITANIUM_RHI_BACKEND "Metal" CACHE STRING "RHI backend")
endif()

add_subdirectory("${THIRD_PARTY_DIR}/TitaniumShaderCompiler")
add_subdirectory("${THIRD_PARTY_DIR}/TitaniumRHI")
add_subdirectory("${THIRD_PARTY_DIR}/glfw")

##assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory("${THIRD_PARTY_DIR}/assimp" EXCLUDE_FROM_ALL)


## ignore Imgui exemples
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${THIRD_PARTY_DIR}/Imgui/examples*"
)
list(FILTER SOURCES EXCLUDE REGEX "\\.slnx$")



set(MATHLIB_USE_INTRINSIC true)
set(MATHLIB_USE_FMA true)

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" TARGET_ARCH)

if(TARGET_ARCH MATCHES "^(x86_64|amd64|x64)$")
    set(MATHLIB_SIMD_EXT AVX)
    # set(MATHLIB_SIMD_EXT AVX2)
elseif(TARGET_ARCH MATCHES "^(aarch64|arm64)$")
    set(MATHLIB_SIMD_EXT SVE)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

add_subdirectory("${THIRD_PARTY_DIR}/MathLib")


add_subdirectory(Sources)