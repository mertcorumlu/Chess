add_executable(chessAttackGen AttackGen.cpp)

add_custom_command(TARGET chessAttackGen POST_BUILD
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/chessAttackGen
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/bin ${CMAKE_CURRENT_SOURCE_DIR}/bin
        COMMENT "Creating lookup table binaries...")

add_library(chessEngine Board.cpp Board.h Attack.cpp Attack.h Position.cpp Position.h)
target_compile_options(chessEngine PRIVATE -mavx512vl -mavx512dq)

target_include_directories(chessEngine PUBLIC .)