cmake_minimum_required(VERSION 3.26)

set(SOURCES
    Asset.hpp
    Asset.cpp
    AssetReference.hpp
    Image.cpp
    Image.hpp
    Texture.cpp
    Texture.hpp
)

add_library(Assets STATIC ${SOURCES})

target_include_directories(
    Assets
    PUBLIC
        "${THIRD_PARTY_DIR}/stb"
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
