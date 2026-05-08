#ifndef TETRIS_RENDERER_H
#define TETRIS_RENDERER_H

#include "Board.h"
#include "Piece.h"
#include <windows.h>

// Console renderer using WriteConsoleOutput for double-buffered, flicker-free display
class Renderer {
public:
    // Layout constants (in console character coordinates)
    static constexpr int BUFFER_WIDTH  = 40;
    static constexpr int BUFFER_HEIGHT = 22;

    // Board drawing region
    static constexpr int BOARD_X = 0;        // left border column
    static constexpr int CELL_X  = 1;        // first cell column
    static constexpr int BOARD_RIGHT_X = 11;  // right border column (BOARD_X + 1 + Board::WIDTH)
    static constexpr int BOARD_Y = 0;         // top border row

    // Info panel start column
    static constexpr int INFO_X = 14;

    Renderer();
    ~Renderer();

    // Initialize console: get handles, set encoding, hide cursor, prepare buffer
    void init();

    // Render a game frame
    void render(const Board& board, const Piece& piece,
                int score, int lines, int level);

    // Render game-over screen
    void renderGameOver(int finalScore);

    // Shutdown: show cursor, restore console
    void shutdown();

private:
    HANDLE hConsole;
    CHAR_INFO buffer[BUFFER_HEIGHT][BUFFER_WIDTH];

    // Clear the entire CHAR_INFO buffer
    void clearBuffer();

    // Draw board border and populated cells
    void drawBoard(const Board& board);

    // Draw the current falling piece (on top of board)
    void drawPiece(const Piece& piece);

    // Draw info panel (score, lines, level, controls)
    void drawInfo(int score, int lines, int level);

    // Draw a string at (x, y) with given color attribute
    void drawString(int x, int y, const char* str, WORD attr);

    // Set a single CHAR_INFO cell
    void setCell(int x, int y, WCHAR ch, WORD attr);

    // Flush the internal buffer to the console
    void flush();
};

#endif // TETRIS_RENDERER_H
