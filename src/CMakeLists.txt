include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(database Database.cpp)
target_include_directories(database PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(SQLite3 REQUIRED)
target_link_libraries(database SQLite::SQLite3)

# -----------------------------------------------------------------------------
# CMake packaging and installation
# -----------------------------------------------------------------------------

file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfig.cmake.in
  "@PACKAGE_INIT@\n\n"
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/DatabaseTargets.cmake\")"
)

configure_package_config_file(
  ${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/database
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfigVersion.cmake
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion
)

install(
  FILES
    Database.h
    DatabaseError.h
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/database
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/DatabaseConfigVersion.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/database
)

install(
  TARGETS database
  EXPORT DatabaseTargets
)

install (
  EXPORT DatabaseTargets
  NAMESPACE database::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/database
  FILE DatabaseTargets.cmake
)

