cmake_minimum_required(VERSION 3.17)
project(centerline)
add_definitions(-std=c++17 -O2 -pedantic-errors)
include_directories(include /usr/include/eigen3)
add_executable(centerline src/shape.cpp src/delaunay.cpp src/centerline.cpp src/main.cpp)
target_link_libraries(centerline m)
