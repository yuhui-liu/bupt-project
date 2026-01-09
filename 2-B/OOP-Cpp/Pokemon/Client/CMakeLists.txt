cmake_minimum_required(VERSION 3.28)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "~/Qt/6.7.2/gcc_64")

find_package(
  Qt6
  COMPONENTS Core Gui Widgets
  REQUIRED)

add_executable(
  Client
  src/main.cpp
  src/mainwindow.cpp
  include/mainwindow.h
  include/ui/mainwindow.ui
  src/Client.cpp
  include/Client.h
  src/secondwindow.cpp
  include/secondwindow.h
  include/ui/secondwindow.ui
  src/viewpokemonwindow.cpp
  include/viewpokemonwindow.h
  include/ui/viewpokemonwindow.ui
  src/battlewindow.cpp
  include/battlewindow.h
  include/ui/battlewindow.ui
  src/viewmypokemonswindow.cpp
  include/viewmypokemonswindow.h
  include/ui/viewmypokemonswindow.ui
  src/wpwindow.cpp
  include/wpwindow.h
  include/ui/wpwindow.ui
  src/losewindow.cpp
  include/losewindow.h
  include/ui/losewindow.ui)
target_link_libraries(Client Qt6::Core Qt6::Gui Qt6::Widgets)

if(WIN32)
  target_link_libraries(Client wsock32 ws2_32)
endif()

if(WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(DEBUG_SUFFIX)
  if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
    set(DEBUG_SUFFIX "d")
  endif()
  set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
  if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
    set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
      set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
    endif()
  endif()
  if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
    add_custom_command(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory
              "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    add_custom_command(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy
        "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
  endif()
  foreach(QT_LIB Core)
    add_custom_command(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy
        "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
  endforeach(QT_LIB)
endif()
