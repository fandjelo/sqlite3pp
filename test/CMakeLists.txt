aux_source_directory(. sources)
add_executable(database_test ${sources})
target_link_libraries(database_test PRIVATE database)

find_package(GTest REQUIRED CONFIG)
target_link_libraries(database_test PRIVATE GTest::gtest GTest::gtest_main)

if(NOT CMAKE_CROSSCOMPILING)
    include(GoogleTest)
    gtest_discover_tests(database_test)
endif()

