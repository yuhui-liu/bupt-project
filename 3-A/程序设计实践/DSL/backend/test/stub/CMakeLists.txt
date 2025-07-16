include(GoogleTest)

add_executable(websocket_test websocket_test.cpp)
target_link_libraries(websocket_test GTest::gtest_main GTest::gmock_main mylib)
gtest_discover_tests(websocket_test)
