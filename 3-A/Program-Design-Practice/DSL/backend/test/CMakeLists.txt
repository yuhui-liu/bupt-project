include(GoogleTest)

file(GLOB TEST_FILES "*.cpp")

foreach(TEST_FILE ${TEST_FILES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_FILE})
  target_link_libraries(${TEST_NAME} GTest::gtest_main mylib)
  gtest_discover_tests(${TEST_NAME})
endforeach()

add_subdirectory(stub)