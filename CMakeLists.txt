cmake_minimum_required(VERSION 2.6)

add_definitions(-Wall -g)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SRC test.cpp)

add_executable(test ${SRC})

