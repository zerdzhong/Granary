cmake_minimum_required(VERSION 3.7)

project(libMultiThread)

set(CMAKE_CXX_STANDARD 11)
set({CMAKE_CXX_FLAGS} -std=c++11)

add_library(libMultiThread thread_base.cpp)