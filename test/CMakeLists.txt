aux_source_directory(. sources)
add_executable(SQLite3ppTest ${sources})
target_link_libraries(SQLite3ppTest PRIVATE SQLite3pp)

find_package(GTest REQUIRED CONFIG)
target_link_libraries(SQLite3ppTest PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(SQLite3ppTest DISCOVERY_MODE PRE_TEST)

