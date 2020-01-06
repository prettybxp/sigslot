cmake_minimum_required(VERSION 2.6)

add_definitions(-Wall -g)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SRC example.cpp)

add_executable(example ${SRC})

target_link_libraries(example -lz -lm -ldl -lpthread)

