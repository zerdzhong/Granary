project(NetworkingTests)

include_directories(${gtests_SOURCE_DIR}/include ${gtests_SOURCE_DIR})

add_executable(NetworkingTests read_connection_test.cpp)

include_directories(../../Networking/Source)

target_link_libraries(NetworkingTests gtest gtest_main)
target_link_libraries(NetworkingTests libNetworking)