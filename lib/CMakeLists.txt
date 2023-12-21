add_library(database INTERFACE)
target_include_directories(database INTERFACE .)

find_package(SQLite3 REQUIRED)
target_link_libraries(database INTERFACE SQLite::SQLite3)

