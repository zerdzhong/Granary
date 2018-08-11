cmake_minimum_required(VERSION 3.7)

project(libNetworking)

add_library(libNetworking ./Source/http_connection.cpp Source/base_connection.cpp Source/base_connection.hpp Source/http_read_connection.cpp Source/http_read_connection.hpp)

include_directories(../MultiThread)
target_link_libraries(libNetworking libMultiThread)

set(use_libcurl_source false)
if (use_libcurl_source)
    add_subdirectory(./Vendor/curl-7.61.0)
    include_directories(./Vendor/curl-7.61.0/include/curl ${curl_SOURCE_DIR}/include ${curl_SOURCE_DIR})
    target_link_libraries(libNetworking libcurl)
else()
    include_directories(./Vendor/curl-7.61.0/include/curl)
    target_link_libraries(libNetworking curl)
endif ()
