cmake_minimum_required(VERSION 3.9)
project(Granary)

set(as_subproject Thread Networking)

macro(find_package)
    message("find_package ${ARGV}")
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

if (MSVC)
    add_compile_options(/W2)
endif()

add_subdirectory(modules/thread)
add_subdirectory(modules/networking)
add_subdirectory(test)
