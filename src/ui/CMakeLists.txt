# UI
add_library(chessUI App.cpp App.h Canvas.cpp Canvas.h Symbol.cpp Symbol.h Frame.cpp Frame.h)

# Include WX Headers
target_include_directories(chessUI PUBLIC /usr/local/lib/wx/include/osx_cocoa-unicode-3.1)
target_include_directories(chessUI PUBLIC /usr/local/include/wx-3.1)

# Include UI Headers
target_include_directories(chessUI PUBLIC .)

# Link WX Libraries
target_link_directories(chessUI PUBLIC /usr/local/lib)
target_link_libraries(chessUI PUBLIC wx_osx_cocoau_qa-3.1 wx_osx_cocoau_core-3.1 wx_baseu-3.1)

# Compile options for WX targets
target_compile_options(chessUI PUBLIC -D__WXOSX_COCOA__)

# Copy images to binary dir
add_custom_command(TARGET chessUI PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        img ${CMAKE_BINARY_DIR}/img)
