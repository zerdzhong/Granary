macro(target_variable_generate)

    set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
    set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
    set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")

    set(target_export_name "${PROJECT_NAME}Targets")
    set(namespace "${PROJECT_NAME}::")

endmacro()

macro(target_package_config)

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            "${version_config}" COMPATIBILITY SameMajorVersion
    )
    configure_package_config_file(
            "Config.cmake.in"
            "${project_config}"
            INSTALL_DESTINATION "${config_install_dir}"
    )

endmacro()

function(target_install arg1)
    # * CMAKE_INSTALL_BINDIR
    # * CMAKE_INSTALL_LIBDIR
    # * CMAKE_INSTALL_INCLUDEDIR

    include(GNUInstallDirs)

    target_variable_generate()
    target_package_config()

    install(
            TARGETS ${arg1}
            EXPORT "${target_export_name}"
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
            EXPORT "${target_export_name}"
            NAMESPACE "${namespace}"
            DESTINATION "${config_install_dir}"
    )

    install(FILES ${ARGN} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

endfunction()