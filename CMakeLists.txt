cmake_minimum_required(VERSION 3.26)

project(TitianiumRHISample VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/ThirdParty")

add_subdirectory("${THIRD_PARTY_DIR}/glfw")
add_subdirectory(Sources)