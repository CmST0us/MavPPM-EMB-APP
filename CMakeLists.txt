cmake_minimum_required(VERSION 3.0)

if(DEBUG STREQUAL "ON")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")
endif()

project(mavppm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(CMAKE_INSTALL_PREFIX "mavppm")

add_subdirectory(src)
