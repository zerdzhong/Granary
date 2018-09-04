cmake_minimum_required(VERSION 3.7)

project(libNetworking)

if(TARGET MultiThread)
    add_library(MultiThread::MultiThread ALIAS MultiThread)
    list(APPEND test_path "$<TARGET_FILE_DIR:Foo::bar>")
else()
    find_package(MultiThread CONFIG REQUIRED)
    message("MultiThread_DIR: ${MultiThread_DIR}")

    list(APPEND test_path "${MultiThread_DIR}/../../../${CMAKE_INSTALL_BINDIR}")
endif()

add_library(Networking src/http_session.cpp src/http_session_task.cpp src/http_session_task.hpp src/http_session_read_task.cpp src/http_session_read_task.hpp)
target_link_libraries(Networking PRIVATE MultiThread::MultiThread)

set(use_libcurl_source false)
if (use_libcurl_source)
    add_subdirectory(./Vendor/curl-7.61.0)
    target_include_directories(Networking PRIVATE ./Vendor/curl-7.61.0/include/curl ${curl_SOURCE_DIR}/include ${curl_SOURCE_DIR})
    target_link_libraries(Networking PRIVATE libcurl)
else()
    target_include_directories(Networking PRIVATE ./Vendor/curl-7.61.0/include/curl)
    target_link_libraries(Networking PRIVATE curl)
endif ()

target_include_directories(Networking
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
