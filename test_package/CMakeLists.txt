cmake_minimum_required(VERSION 3.18)
project(PackageTest CXX)

find_package(sqlite3pp CONFIG REQUIRED)

add_executable(pkgtest pkgtest.cpp)
target_link_libraries(pkgtest sqlite3pp::sqlite3pp)
