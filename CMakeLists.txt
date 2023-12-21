cmake_minimum_required(VERSION 3.18)
project(database)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
