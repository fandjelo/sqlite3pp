add_library(database INTERFACE)
target_include_directories(database INTERFACE .)
find_package(SQLite3 REQUIRED)
target_link_libraries(database INTERFACE SQLite::SQLite3)

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE database)

