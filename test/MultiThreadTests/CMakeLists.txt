project(MultiThreadTests)

add_executable(MultiThreadTests
        thread_base_test.cpp
        producer_consumer_test.cpp
        timer_test.cpp
        thread_loop_test.cpp)

if (NOT MSVC)
    find_package(Thread CONFIG REQUIRED)
endif ()

target_compile_features(MultiThreadTests PRIVATE cxx_std_14)

target_link_libraries(MultiThreadTests gtest gtest_main)
target_link_libraries(MultiThreadTests Thread::Thread)

add_test(NAME RunMultiThreadTests COMMAND MultiThreadTests)