cmake_minimum_required(VERSION 2.8)

project(RingQueue)

set(ARCH 1)  # 0: aarch64, 1: current arch
set(DEBUG 1)

if(ARCH STREQUAL "0")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")

    message("build release version for aarch64")
endif()

add_compile_options(-std=c++11 -Wall)

if(DEBUG STREQUAL "0")
    add_compile_options(-O3)
    set(CMAKE_BUILD_TYPE "Release")
else()
    SET(CMAKE_BUILD_TYPE "Debug")
    add_compile_options(-O0 -g3 -ggdb3)
endif()

add_executable(test_RingQueue test_RingQueue.cpp)
target_link_libraries(test_RingQueue pthread)

add_executable(test_LatestFixedQueue test_LatestFixedQueue.cpp)
