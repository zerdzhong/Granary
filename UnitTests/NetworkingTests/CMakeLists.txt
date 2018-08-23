project(NetworkingTests)

if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories(BEFORE SYSTEM
            "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
else()
    target_include_directories(gmock_main SYSTEM BEFORE INTERFACE
            "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
endif()

add_executable(NetworkingTests session_read_task_test.cpp)

include_directories(../../Networking/Source)

target_link_libraries(NetworkingTests  gmock_main)
target_link_libraries(NetworkingTests libNetworking)