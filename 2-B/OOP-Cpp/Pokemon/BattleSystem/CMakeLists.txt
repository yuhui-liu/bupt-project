cmake_minimum_required(VERSION 3.27)
project(BattleSystem)

set(CMAKE_CXX_STANDARD 17)

# 静态链接，使得单个exe可以执行
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(BattleSystem src/main.cpp
        src/Pokemon.cpp
        include/Pokemon.h
        src/Tools.cpp
        include/Tools.h
        src/Server.cpp
        include/Server.h
        include/nlohmann/json.hpp
)

# Windows下需要链接到wsock32和ws2_32：Windows的socket实现
if(WIN32)
    target_link_libraries(BattleSystem wsock32 ws2_32)
endif()
