cmake_minimum_required(VERSION 3.16)
project(TetrisConsole VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files
set(SOURCES
    main.cpp
    Game.cpp
    Board.cpp
    Piece.cpp
    Renderer.cpp
    Input.cpp
)

add_executable(tetris ${SOURCES})

# Link Windows libraries for console API
if(WIN32)
    target_link_libraries(tetris PRIVATE)
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(tetris PRIVATE /W4 /EHsc)
elseif(MINGW)
    target_compile_options(tetris PRIVATE -Wall -Wextra -O2)
endif()
