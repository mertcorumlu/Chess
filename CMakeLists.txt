cmake_minimum_required(VERSION 3.20)
project(Chess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mavx2 -msse2 -Wall -Wextra -mbmi2")

add_subdirectory(ui)
add_subdirectory(engine)

add_executable(chess main.cpp)
target_link_libraries(chess chessUI)
target_link_libraries(chess chessEngine)

add_executable(test test.cpp)
target_link_libraries(test chessEngine)