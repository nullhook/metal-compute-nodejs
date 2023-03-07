cmake_minimum_required(VERSION 3.22)

project(font-renderer-metal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fno-objc-arc")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/metal-cmake)

add_library(metal-compute SHARED "main.cpp")
target_link_libraries(metal-compute METAL_CPP)
