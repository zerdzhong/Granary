cmake_minimum_required(VERSION 3.7)

project(Thread VERSION 1.0.0)

add_library(Thread src/thread_base.cpp)
add_library(Thread::Thread ALIAS Thread)

target_compile_features(Thread PRIVATE cxx_std_14)

target_include_directories(Thread
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        src
        )

include(../../cmake/target_install.cmake)
target_install(Thread include/thread_base.hpp)