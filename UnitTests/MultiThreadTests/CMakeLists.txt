project(MultiThreadTests)

include_directories(${gtests_SOURCE_DIR}/include ${gtests_SOURCE_DIR})

add_executable(MultiThreadTests thread_base_test.cpp producer_consumer_test.cpp)


find_package(MultiThread CONFIG REQUIRED)

target_compile_features(MultiThreadTests PRIVATE cxx_std_11)

target_link_libraries(MultiThreadTests gtest gtest_main)
target_link_libraries(MultiThreadTests MultiThread::MultiThread)
