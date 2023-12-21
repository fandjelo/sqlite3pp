aux_source_directory(${CMAKE_CURRENT_LIST_DIR} sqlite3ppSources)
add_library(sqlite3pp ${sqlite3ppSources})
target_link_libraries(sqlite3pp PUBLIC sqlite3pp_api)

find_package(SQLite3 REQUIRED)
target_link_libraries(sqlite3pp PRIVATE SQLite::SQLite3)

