cmake_minimum_required(VERSION 3.7)

project(MultiThread VERSION 1.0.0)

add_library(MultiThread src/thread_base.cpp)
add_library(MultiThread::MultiThread ALIAS MultiThread)

target_compile_features(MultiThread PRIVATE cxx_std_11)

target_include_directories(MultiThread
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

include(../cmake/target_install.cmake)
target_install(MultiThread include/thread_base.hpp)