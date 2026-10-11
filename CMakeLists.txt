cmake_minimum_required(VERSION 3.16)
project(GpuFabric LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sim_core
    src/sim/simulator.cpp
    src/sim/rdma.cpp
    src/network/topology.cpp
    src/network/router.cpp
)

target_include_directories(sim_core PUBLIC src)
target_compile_options(sim_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(gpu_fabric src/main.cpp)
target_link_libraries(gpu_fabric PRIVATE sim_core)

add_executable(test_link tests/test_link.cpp)
target_link_libraries(test_link PRIVATE sim_core)

add_executable(test_ecmp tests/test_ecmp.cpp)
target_link_libraries(test_ecmp PRIVATE sim_core)

add_executable(test_ecmp_fat_tree tests/test_ecmp_fat_tree.cpp)
target_link_libraries(test_ecmp_fat_tree PRIVATE sim_core)

enable_testing()
add_test(NAME link COMMAND test_link)
add_test(NAME ecmp COMMAND test_ecmp)
add_test(NAME ecmp_fat_tree COMMAND test_ecmp_fat_tree)