#
# MIT License
#
# Copyright (c) 2023 Filipp Andjelo
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
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
    VERSION ${CMAKE_PROJECT_VERSION}
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

