cmake_minimum_required(VERSION 3.18)
project(database)

option(SQLITE3PP_WITH_TESTS "Build with tests" TRUE)
option(SQLITE3PP_WITH_EXAMPLES "Build with examples" TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
enable_testing()

add_subdirectory(src)

