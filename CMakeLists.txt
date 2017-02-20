list( APPEND CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -pedantic")

project(simple_ray_tracer)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})


