cmake_minimum_required(VERSION 3.16)

project(FlowTest)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -pedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(-fprofile-arcs -ftest-coverage -lgcov)
endif()

add_executable(FlowTest "")

target_include_directories(FlowTest PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
)

target_sources(FlowTest
    PRIVATE
        source/main.cpp
        source/component_combine_tests.cpp
        source/component_invert_tests.cpp
        source/component_toggle_tests.cpp
        source/inoutport_tests.cpp
        source/queue_tests.cpp
        source/trigger_tests.cpp
        source/component_convert_tests.cpp
        source/component_split_tests.cpp
        source/component_updowncounter_tests.cpp
        source/pool_tests.cpp
        source/reactor_tests.cpp
        source/component_counter_tests.cpp
        source/component_timer_tests.cpp
        source/connection_tests.cpp
        source/port_tests.cpp
        source/testreactor_tests.cpp
        ${PROJECT_BINARY_DIR}/source/flow/platform_cpputest.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(FlowTest 
    Threads::Threads
    CONAN_PKG::Flow
    CONAN_PKG::CppUTest
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(FlowTest 
        gcov
    )
endif()
