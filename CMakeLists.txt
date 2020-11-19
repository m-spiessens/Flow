cmake_minimum_required(VERSION 3.16)

project(Flow)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -pedantic -fprofile-arcs -ftest-coverage)
endif()

configure_file(include/version.h.in version.h)

add_library(Flow "")
target_include_directories(Flow PUBLIC 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include")

target_sources(Flow
    PRIVATE
        source/components.cpp
        source/flow.cpp
        source/reactor.cpp
)
