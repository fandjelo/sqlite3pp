cmake_minimum_required(VERSION 3.18)
project(database)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

option(SQLITE3PP_WITH_TESTS "Build with tests" TRUE)
option(SQLITE3PP_WITH_EXAMPLES "Build with examples" TRUE)

add_subdirectory(src)

if(SQLITE3PP_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

if(SQLITE3PP_WITH_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

