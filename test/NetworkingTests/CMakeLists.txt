project(NetworkingTests)

find_package(Networking CONFIG REQUIRED)

add_executable(NetworkingTests
        session_read_task_test.cpp
        session_test.cpp
        session_mock_listener.cpp
        session_config_test.cpp
        mock_read_task.cpp)

target_compile_features(NetworkingTests PRIVATE cxx_std_14)

target_link_libraries(NetworkingTests gmock_main)
target_link_libraries(NetworkingTests Networking::Networking)

add_test(NAME RunNetworkingTests COMMAND NetworkingTests)