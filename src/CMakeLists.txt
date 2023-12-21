include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} SQLite3ppSources)
add_library(SQLite3pp ${SQLite3ppSources})
target_include_directories(SQLite3pp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(SQLite3 REQUIRED)
target_link_libraries(SQLite3pp SQLite::SQLite3)

# -----------------------------------------------------------------------------
# CMake packaging and installation
# -----------------------------------------------------------------------------

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/SQLite3ppConfig.cmake.in
    "@PACKAGE_INIT@\n\n"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/SQLite3ppTargets.cmake\")"
)

configure_package_config_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SQLite3ppConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SQLite3ppConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SQLite3pp
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SQLite3ppConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

install(
    DIRECTORY include/SQLite3pp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/SQLite3ppConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/SQLite3ppConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/SQLite3pp
)

install(
    TARGETS SQLite3pp 
    EXPORT SQLite3ppTargets
)

install(
    EXPORT SQLite3ppTargets
    NAMESPACE SQLite3pp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SQLite3pp
    FILE SQLite3ppTargets.cmake
)

