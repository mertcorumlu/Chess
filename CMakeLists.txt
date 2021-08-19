cmake_minimum_required(VERSION 3.20)
project(Chess)

include_directories(/usr/local/lib/wx/include/osx_cocoa-unicode-3.1)
include_directories(/usr/local/include/wx-3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -L/usr/local/lib  -framework IOKit -framework Carbon -framework Cocoa -framework QuartzCore -framework AudioToolbox -framework System -framework OpenGL -lwx_osx_cocoau_xrc-3.1 -lwx_osx_cocoau_html-3.1 -lwx_osx_cocoau_qa-3.1 -lwx_osx_cocoau_core-3.1 -lwx_baseu_xml-3.1 -lwx_baseu_net-3.1 -lwx_baseu-3.1 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXMAC__ -D__WXOSX__ -D__WXOSX_COCOA__ -Wno-unused-command-line-argument")
add_executable(chess main.cpp main.h Board.cpp Board.h Piece.cpp Piece.h ChessFrame.cpp ChessFrame.h)

add_custom_command(TARGET chess PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/img $<TARGET_FILE_DIR:chess>/img)