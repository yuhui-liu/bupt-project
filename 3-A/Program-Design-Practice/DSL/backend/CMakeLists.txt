cmake_minimum_required(VERSION 3.30)

project(
  DSL
  VERSION 1.0
  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB INCLUDE_FILES CONFIGURE_DEPENDS include/*.hpp)

include_directories(include)
find_package(
  Boost 1.82.0
  COMPONENTS program_options
  REQUIRED)
find_package(spdlog)

add_executable(DSL ${SOURCE_FILES} ${INCLUDE_FILES})
target_link_libraries(DSL Boost::program_options spdlog::spdlog sqlite3)

##### test####
enable_testing()
add_library(mylib STATIC ${SOURCE_FILES})
target_link_libraries(mylib Boost::program_options spdlog::spdlog sqlite3)
add_subdirectory(third_party)
add_subdirectory(test)
