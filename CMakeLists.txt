cmake_minimum_required(VERSION 3.14)
project(database)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
aux_source_directory(. sources)
add_executable(database ${sources})

find_package(SQLite3 REQUIRED)
target_link_libraries(database PRIVATE SQLite::SQLite3)
