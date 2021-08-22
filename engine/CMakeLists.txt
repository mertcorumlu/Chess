add_library(chessEngine Board.cpp Board.h Attack.h)

target_include_directories(chessEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})