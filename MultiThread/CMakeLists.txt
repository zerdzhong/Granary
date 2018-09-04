cmake_minimum_required(VERSION 3.7)

project(libMultiThread)

set(CMAKE_CXX_STANDARD 11)
set({CMAKE_CXX_FLAGS} -std=c++11)

add_library(MultiThread src/thread_base.cpp)

target_include_directories(MultiThread
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )