project(MultiThreadTests)

include_directories(${gtests_SOURCE_DIR}/include ${gtests_SOURCE_DIR})

add_executable(MultiThreadTests thread_base_test.cpp producer_consumer_test.cpp)

if(TARGET MultiThread)
    add_library(MultiThread::MultiThread ALIAS MultiThread)
    message("MultiThread_DIR: ${MultiThread_DIR}")
    list(APPEND test_path "$<TARGET_FILE_DIR:Foo::bar>")
else()
    find_package(MultiThread CONFIG REQUIRED)
    message("MultiThread_DIR: ${MultiThread_DIR}")

    list(APPEND test_path "${MultiThread_DIR}/../../../${CMAKE_INSTALL_BINDIR}")
endif()

target_link_libraries(MultiThreadTests gtest gtest_main)
target_link_libraries(MultiThreadTests MultiThread::MultiThread)
