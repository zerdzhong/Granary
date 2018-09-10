project(UnitTests)

include(incorporate_google_test.cmake)

add_subdirectory(MultiThreadTests)
add_subdirectory(NetworkingTests)