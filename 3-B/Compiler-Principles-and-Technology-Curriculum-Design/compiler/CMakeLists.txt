cmake_minimum_required(VERSION 3.30)

project(
  COMPILER
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB ALL_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB INCLUDE_FILES CONFIGURE_DEPENDS include/*.hpp)

include_directories(include)

set(SOURCE_FILES ${ALL_SOURCE_FILES})
list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main_submit.cpp)
add_executable(COMPILER ${SOURCE_FILES} ${INCLUDE_FILES})

# for submit
set(SUBMIT_SOURCE_FILES ${ALL_SOURCE_FILES})
list(REMOVE_ITEM SUBMIT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(SUBMIT ${SUBMIT_SOURCE_FILES} ${INCLUDE_FILES})

# for test
enable_testing()
add_library(mylib STATIC ${SOURCE_FILES})
add_subdirectory(third_party)
add_subdirectory(test)
