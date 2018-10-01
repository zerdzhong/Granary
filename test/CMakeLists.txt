project(UnitTests)

enable_testing()



add_subdirectory(3rdparty/googletest-release-1.8.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(MultiThreadTests)
add_subdirectory(NetworkingTests)