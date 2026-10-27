cmake_minimum_required(VERSION 3.26)

set(EXECUTABLE_NAME Triangle)

add_executable(
    ${EXECUTABLE_NAME}
    main.cpp
)   

list(APPEND EXECUTABLE_NAMES ${EXECUTABLE_NAME})

set(EXECUTABLE_NAMES "${EXECUTABLE_NAMES}" PARENT_SCOPE)