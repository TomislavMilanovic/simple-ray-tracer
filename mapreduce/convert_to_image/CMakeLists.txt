list( APPEND CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -pedantic -o3 -fpermissive")
cmake_minimum_required(VERSION 2.8)

project(convert_to_image)

file(GLOB_RECURSE SRC_LIST
    ../../lib/lodepng/lodepng.h
    ../../lib/lodepng/lodepng.cpp
)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
