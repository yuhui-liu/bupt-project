include(GoogleTest)

file(GLOB TEST_FILES "*.cpp")

foreach(TEST_FILE ${TEST_FILES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_FILE})

  # Add this line to define the macro for this specific test executable
  target_compile_definitions(${TEST_NAME} PRIVATE ENABLE_GTEST_FRIENDS)

  target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main mylib)
  gtest_discover_tests(${TEST_NAME})
endforeach()
