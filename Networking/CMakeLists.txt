cmake_minimum_required(VERSION 3.7)
project(Networking VERSION 1.0.0)

if(TARGET MultiThread)
    add_library(MultiThread::MultiThread ALIAS MultiThread)
else()
    find_package(MultiThread CONFIG REQUIRED)
    message("MultiThread_DIR: ${MultiThread_DIR}")
endif()


add_library(Networking src/http_session.cpp src/http_session_task.cpp src/http_session_task.hpp src/http_session_read_task.cpp src/http_session_read_task.hpp)
target_link_libraries(Networking PRIVATE MultiThread::MultiThread)

set(use_libcurl_source false)
if (use_libcurl_source)
    add_subdirectory(./Vendor/curl-7.61.0)
    target_include_directories(Networking PRIVATE ./Vendor/curl-7.61.0/include/curl ${curl_SOURCE_DIR}/include ${curl_SOURCE_DIR})
    target_link_libraries(Networking Pconfigure_fileRIVATE libcurl)
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

# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR

include(GNUInstallDirs)

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)

# Use:
# * PROJECT_VERSION
write_basic_package_version_file(
        "${version_config}" COMPATIBILITY SameMajorVersion
)

# Use:
# * TARGETS_EXPORT_NAME
# * PROJECT_NAME
configure_package_config_file(
        "Config.cmake.in"
        "${project_config}"
        INSTALL_DESTINATION "${config_install_dir}"
)

install(
        TARGETS Networking
        EXPORT "${TARGETS_EXPORT_NAME}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
        FILES "${project_config}" "${version_config}"
        DESTINATION "${config_install_dir}"
)

install(
        EXPORT "${TARGETS_EXPORT_NAME}"
        NAMESPACE "${namespace}"
        DESTINATION "${config_install_dir}"
)

install(FILES include/http_session.hpp include/http_session_read_task.hpp include/http_session_task.hpp DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
