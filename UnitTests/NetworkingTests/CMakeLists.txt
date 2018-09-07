project(NetworkingTests)

if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories(BEFORE SYSTEM
            "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
else()
    target_include_directories(gmock_main SYSTEM BEFORE INTERFACE
            "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
endif()

if(TARGET Networking)
    add_library(libNetworking::Networking ALIAS Networking)
    list(APPEND test_path "$<TARGET_FILE_DIR:Foo::bar>")
else()
    find_package(Networking CONFIG REQUIRED)
    message("MultiThread_DIR: ${libNetworking_DIR}")

    list(APPEND test_path "${libNetworking_DIR}/../../../${CMAKE_INSTALL_BINDIR}")

endif()

add_executable(NetworkingTests session_read_task_test.cpp session_test.cpp session_mock_listener.cpp)

target_compile_features(NetworkingTests PRIVATE cxx_std_11)

target_link_libraries(NetworkingTests gmock_main)
target_link_libraries(NetworkingTests libNetworking::Networking)