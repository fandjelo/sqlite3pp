add_library(database Database.cpp)
target_include_directories(database PUBLIC .)

find_package(SQLite3 REQUIRED)
target_link_libraries(database INTERFACE SQLite::SQLite3)

