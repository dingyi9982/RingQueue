cmake_minimum_required(VERSION 2.8)

project(RingQueue)

add_compile_options(-std=c++11 -Wall)

# SET(CMAKE_BUILD_TYPE "Debug")
# add_compile_options(-O0 -g3 -ggdb3)

add_executable(test_RingQueue test_RingQueue.cpp)
target_link_libraries(test_RingQueue pthread)
