cmake_minimum_required(VERSION 3.16)

project(Flow)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -pedantic)

if(COVERAGE STREQUAL "YES")
    add_compile_options(-fprofile-arcs -ftest-coverage)
endif()

add_library(Flow "")
target_include_directories(Flow PUBLIC 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include")

target_sources(Flow
    PRIVATE
        source/flow/components.cpp
        source/flow/flow.cpp
        source/flow/reactor.cpp
)
