cmake_minimum_required(VERSION 3.12)
project(Granary)

set(as_subproject MultiThread)

macro(find_package)
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

add_subdirectory(MultiThread)
add_subdirectory(Networking)
add_subdirectory(UnitTests)
