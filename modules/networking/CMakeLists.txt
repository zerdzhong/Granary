cmake_minimum_required(VERSION 3.7)
project(Networking VERSION 1.0.0)

add_library(Networking
        src/http_session.cpp
        src/http_session_task.cpp
        src/http_session_read_task.cpp
        src/http_session_config.cpp
        src/http_curl_wrapper.cpp)

add_library(Networking::Networking ALIAS Networking)
target_link_libraries(Networking PRIVATE Thread::Thread)

target_compile_features(Networking PRIVATE cxx_std_14)

set(use_libcurl_source false)

if (use_libcurl_source)
    add_subdirectory(../../3rdparty/curl-7.61.0 curl.out)
    target_include_directories(Networking PRIVATE ../../3rdparty/curl-7.61.0/include/curl ${curl_SOURCE_DIR}/include ${curl_SOURCE_DIR})
    target_link_libraries(Networking PRIVATE libcurl)
else ()
    find_package(Curl REQUIRED)
    if(NOT CURL_FOUND)
        message(SEND_ERROR "Cannot find Curl library")
        return()
    else()
        target_include_directories(Networking PRIVATE ${CURL_INCLUDE_DIR})
    endif()

    target_link_libraries(Networking PRIVATE ${CURL_LIBRARY})
endif ()

target_include_directories(Networking
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        src
        )

include(../../cmake/target_install.cmake)
target_install(Networking
        include/http_session_task.hpp
        include/http_session_read_task.hpp
        include/http_session.hpp
        include/http_session_config.hpp
        )
