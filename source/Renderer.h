#ifndef TETRIS_RENDERER_H
#define TETRIS_RENDERER_H

#include "Board.h"
#include "Piece.h"
#include <windows.h>

class Renderer {
public:
    static constexpr int BUFFER_WIDTH  = 40;
    static constexpr int BUFFER_HEIGHT = 22;

    static constexpr int BOARD_X = 0;
    static constexpr int CELL_X  = 1;
    static constexpr int BOARD_RIGHT_X = 11;
    static constexpr int BOARD_Y = 0;
    static constexpr int INFO_X = 14;

    Renderer();
    ~Renderer();

    void init();

    // Render a normal game frame
    void render(const Board& board, const Piece& piece,
                int score, int highScore, int lines, int level);

    // Render a paused game (shows the board without the current piece, + pause overlay)
    void renderPaused(const Board& board,
                      int score, int highScore, int lines, int level);

    // Render game-over screen
    void renderGameOver(int finalScore, int highScore);

    void shutdown();

private:
    HANDLE hConsole;
    CHAR_INFO buffer[BUFFER_HEIGHT][BUFFER_WIDTH];

    void clearBuffer();
    void drawBoard(const Board& board);
    void drawPiece(const Piece& piece);
    void drawInfo(int score, int highScore, int lines, int level);

    // Draw a centered overlay string on the board area (for pause / game-over overlays)
    void drawBoardOverlay(const char* line1, const char* line2, WORD attr);

    void drawString(int x, int y, const char* str, WORD attr);
    void setCell(int x, int y, WCHAR ch, WORD attr);
    void flush();
};

#endif // TETRIS_RENDERER_H
