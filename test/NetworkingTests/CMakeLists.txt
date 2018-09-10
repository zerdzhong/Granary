project(NetworkingTests)

find_package(Networking CONFIG REQUIRED)

add_executable(NetworkingTests
        session_read_task_test.cpp
        session_test.cpp
        session_mock_listener.cpp)

target_compile_features(NetworkingTests PRIVATE cxx_std_11)

target_link_libraries(NetworkingTests gmock_main)
target_link_libraries(NetworkingTests Networking::Networking)