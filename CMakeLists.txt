cmake_minimum_required(VERSION 3.16)
project(GpuFabric LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Shared simulator implementation
add_library(sim_core
    src/sim/simulator.cpp
    src/network/topology.cpp
)

target_include_directories(sim_core PUBLIC src)
target_compile_options(sim_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(gpu_fabric src/main.cpp)
target_link_libraries(gpu_fabric PRIVATE sim_core)

add_executable(test_scheduler tests/test_scheduler.cpp)
target_link_libraries(test_scheduler PRIVATE sim_core)

enable_testing()
add_test(NAME scheduler COMMAND test_scheduler)