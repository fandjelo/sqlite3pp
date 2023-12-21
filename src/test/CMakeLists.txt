aux_source_directory(. sources)
add_executable(sqlite3ppTest ${sources})
target_link_libraries(sqlite3ppTest PRIVATE sqlite3pp)

find_package(GTest REQUIRED CONFIG)
target_link_libraries(sqlite3ppTest PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(sqlite3ppTest DISCOVERY_MODE PRE_TEST)

