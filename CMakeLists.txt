cmake_minimum_required(VERSION 3.20)
project(Chess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=x86-64 -mavx2 -msse2 -Wall -Wextra -mbmi2")

include(CPM.cmake)

enable_testing()

CPMAddPackage("gh:onqtam/doctest#2.4.5")
set(CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake ${CMAKE_MODULE_PATH})

add_subdirectory(src)
add_subdirectory(test EXCLUDE_FROM_ALL)