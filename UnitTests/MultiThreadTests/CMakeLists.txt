project(MultiThreadTests)

include_directories(${gtests_SOURCE_DIR}/include ${gtests_SOURCE_DIR})

add_executable(MultiThreadTests thread_base_test.cpp producer_consumer_test.cpp)

include_directories(../../MultiThread)

target_link_libraries(MultiThreadTests gtest gtest_main)
target_link_libraries(MultiThreadTests libMultiThread)
