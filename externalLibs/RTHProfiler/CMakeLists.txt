#builds the RTHProfiler

cmake_minimum_required(VERSION 2.6)

set(SRC_FILES
    ./source/profile.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/
)

add_library( RTHProfiler ${SRC_FILES} )

