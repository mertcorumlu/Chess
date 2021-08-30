add_subdirectory(ui)
add_subdirectory(engine)

add_executable(chess main.cpp)
target_link_libraries(chess chessUI)
target_link_libraries(chess chessEngine)

add_executable(draft draft.cpp)
target_link_libraries(draft chessEngine)