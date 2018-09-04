cmake_minimum_required(VERSION 3.12)
project(Granary)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(MultiThread)
add_subdirectory(Networking)
add_subdirectory(UnitTests)
