cmake_minimum_required(VERSION 3.16)

project(Flow)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -pedantic)

add_library(Flow "")
target_include_directories(Flow
PUBLIC 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
)

target_sources(Flow
PRIVATE
    source/flow/components.cpp
    source/flow/flow.cpp
    source/flow/reactor.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(FlowCoverage "")

target_compile_options(FlowCoverage
PRIVATE
    -fprofile-arcs
    -ftest-coverage
)

target_link_options(FlowCoverage
PRIVATE
    -fprofile-arcs
    -ftest-coverage
    -lgcov
)

target_include_directories(FlowCoverage
PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/test_package/include"
)

target_sources(FlowCoverage
PRIVATE
    source/flow/platform_cpputest.cpp
    source/flow/components.cpp
    source/flow/flow.cpp
    source/flow/reactor.cpp
    test_package/source/main.cpp
    test_package/source/component_combine_tests.cpp
    test_package/source/component_invert_tests.cpp
    test_package/source/component_toggle_tests.cpp
    test_package/source/inoutport_tests.cpp
    test_package/source/queue_tests.cpp
    test_package/source/trigger_tests.cpp
    test_package/source/component_convert_tests.cpp
    test_package/source/component_split_tests.cpp
    test_package/source/component_updowncounter_tests.cpp
    test_package/source/pool_tests.cpp
    test_package/source/reactor_tests.cpp
    test_package/source/component_counter_tests.cpp
    test_package/source/component_timer_tests.cpp
    test_package/source/connection_tests.cpp
    test_package/source/port_tests.cpp
    test_package/source/testreactor_tests.cpp
)

target_link_libraries(FlowCoverage 
    Threads::Threads
    CONAN_PKG::CppUTest
)
