cmake_minimum_required(VERSION 3.7)
project(Base VERSION 1.0.0)

add_library(Base checks.cpp)
add_library(Base::Base ALIAS Base)

target_compile_features(Base PRIVATE cxx_std_14)

target_include_directories(Base
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        src
        )

if(UNIX)
    target_compile_definitions(Base PRIVATE POSIX=1)
endif()

if(UNIX)
    target_compile_definitions(Base PRIVATE POSIX=1)
endif()

if(MSVC OR MSYS OR MINGW)
    target_compile_definitions(Base PRIVATE WIN=1)
endif()
