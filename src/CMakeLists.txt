include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} sqlite3ppSources)
add_library(sqlite3pp ${sqlite3ppSources})
target_include_directories(sqlite3pp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(SQLite3 REQUIRED)
target_link_libraries(sqlite3pp SQLite::SQLite3)

# -----------------------------------------------------------------------------
# CMake packaging and installation
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

install(
    DIRECTORY include/sqlite3pp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ppConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3pp
)

install(
    TARGETS sqlite3pp 
    EXPORT sqlite3ppTargets
)

install(
    EXPORT sqlite3ppTargets
    NAMESPACE sqlite3pp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3pp
    FILE sqlite3ppTargets.cmake
)

