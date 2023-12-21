include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(api)
add_subdirectory(impl)

if(SQLITE3PP_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

if(SQLITE3PP_WITH_TESTS)
  add_subdirectory(test)
endif()

# -----------------------------------------------------------------------------
# Generate config files
# -----------------------------------------------------------------------------

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfig.cmake.in
    "@PACKAGE_INIT@\n\n"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/sqlite3ppTargets.cmake\")"
)

configure_package_config_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3pp
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

# -----------------------------------------------------------------------------
# Packaging and installation
# -----------------------------------------------------------------------------

install(
    TARGETS sqlite3pp sqlite3pp_api
    EXPORT sqlite3ppTargets
)

install(
    EXPORT sqlite3ppTargets
    NAMESPACE sqlite3pp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3pp
    FILE sqlite3ppTargets.cmake
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3pp
)

