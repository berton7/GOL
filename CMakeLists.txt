project(GOL)

cmake_minimum_required(VERSION 3.18)

set(SOURCES
    src/Main.cpp
    src/Game.cpp
    src/Cell.cpp
    src/Logger.cpp)
add_executable(GameOfLife ${SOURCES})
set_target_properties(GameOfLife PROPERTIES CXX_STANDARD 17)
target_link_libraries(GameOfLife SDL2)