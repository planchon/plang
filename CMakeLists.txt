cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

project(plang)

include_directories(include)

add_executable(plang src/plang.cpp src/token.cpp src/ast.cpp src/error.cpp)