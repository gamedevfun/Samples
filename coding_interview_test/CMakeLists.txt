cmake_minimum_required(VERSION 3.15)
project(TestProject)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestProject main.cpp)
add_executable(Elevator Elevator.cpp)