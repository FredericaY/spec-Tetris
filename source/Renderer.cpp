#include "Renderer.h"
#include "Color.h"
#include <cstdio>
#include <cstring>

Renderer::Renderer()
    : hConsole(nullptr)
{
}

Renderer::~Renderer()
{
    shutdown();
}

void Renderer::init()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        hConsole = nullptr;
        return;
    }

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    SetConsoleTitleW(L"Tetris");

    COORD bufferSize = { BUFFER_WIDTH, BUFFER_HEIGHT + 1 };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    SMALL_RECT windowRect = { 0, 0, BUFFER_WIDTH - 1, BUFFER_HEIGHT + 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowRect);

    clearBuffer();
}

void Renderer::shutdown()
{
    if (hConsole) {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        COORD pos = { 0, BUFFER_HEIGHT };
        SetConsoleCursorPosition(hConsole, pos);
    }
}

void Renderer::clearBuffer()
{
    for (int y = 0; y < BUFFER_HEIGHT; ++y) {
        for (int x = 0; x < BUFFER_WIDTH; ++x) {
            buffer[y][x].Char.UnicodeChar = L' ';
            buffer[y][x].Attributes = 0;
        }
    }
}

void Renderer::setCell(int x, int y, WCHAR ch, WORD attr)
{
    if (x >= 0 && x < BUFFER_WIDTH && y >= 0 && y < BUFFER_HEIGHT) {
        buffer[y][x].Char.UnicodeChar = ch;
        buffer[y][x].Attributes = attr;
    }
}

void Renderer::drawString(int x, int y, const char* str, WORD attr)
{
    int col = x;
    while (*str && col < BUFFER_WIDTH) {
        setCell(col, y, static_cast<WCHAR>(*str), attr);
        ++str;
        ++col;
    }
}

void Renderer::drawBoardOverlay(const char* line1, const char* line2, WORD attr)
{
    // Center text in the 10-wide board area (columns 1..10)
    int boardCenterCol = CELL_X + Board::WIDTH / 2;
    int boardCenterRow = BOARD_Y + 1 + Board::HEIGHT / 2;

    int len1 = static_cast<int>(std::strlen(line1));
    int len2 = static_cast<int>(std::strlen(line2));

    int x1 = boardCenterCol - len1 / 2;
    int x2 = boardCenterCol - len2 / 2;

    // Dim the board area first: reduce brightness of existing cells
    for (int by = 0; by < Board::HEIGHT; ++by) {
        int screenY = BOARD_Y + 1 + by;
        for (int bx = 0; bx < Board::WIDTH; ++bx) {
            int screenX = CELL_X + bx;
            WORD existing = buffer[screenY][screenX].Attributes;
            // Strip INTENSITY to dim the board behind the overlay
            WORD dimmed = existing & ~FOREGROUND_INTENSITY;
            // If no color, give a subtle dark background
            if ((dimmed & 0x0F) == 0) {
                dimmed = 0;
            }
            buffer[screenY][screenX].Attributes = dimmed;
        }
    }

    // Draw overlay text
    drawString(x1, boardCenterRow - 1, line1, attr);
    drawString(x2, boardCenterRow, line2, attr);
}

void Renderer::drawBoard(const Board& board)
{
    const WORD borderColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

    setCell(BOARD_X, BOARD_Y, L'+', borderColor);
    for (int x = 0; x < Board::WIDTH; ++x) {
        setCell(CELL_X + x, BOARD_Y, L'-', borderColor);
    }
    setCell(BOARD_RIGHT_X, BOARD_Y, L'+', borderColor);

    for (int by = 0; by < Board::HEIGHT; ++by) {
        int screenY = BOARD_Y + 1 + by;
        setCell(BOARD_X, screenY, L'|', borderColor);
        setCell(BOARD_RIGHT_X, screenY, L'|', borderColor);

        for (int bx = 0; bx < Board::WIDTH; ++bx) {
            if (board.isOccupied(bx, by)) {
                Color c = board.getColor(bx, by);
                WORD fg = getConsoleForeground(c);
                setCell(CELL_X + bx, screenY, L'#', fg);
            } else {
                setCell(CELL_X + bx, screenY, L' ', 0);
            }
        }
    }

    int bottomY = BOARD_Y + 1 + Board::HEIGHT;
    setCell(BOARD_X, bottomY, L'+', borderColor);
    for (int x = 0; x < Board::WIDTH; ++x) {
        setCell(CELL_X + x, bottomY, L'-', borderColor);
    }
    setCell(BOARD_RIGHT_X, bottomY, L'+', borderColor);
}

void Renderer::drawPiece(const Piece& piece)
{
    int px = piece.getX();
    int py = piece.getY();
    WORD fg = getConsoleForeground(piece.getColor());

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (piece.isFilled(r, c)) {
                int bx = px + c;
                int by = py + r;

                if (bx >= 0 && bx < Board::WIDTH && by >= 0 && by < Board::HEIGHT) {
                    int screenX = CELL_X + bx;
                    int screenY = BOARD_Y + 1 + by;
                    setCell(screenX, screenY, L'#', fg);
                }
            }
        }
    }
}

void Renderer::drawInfo(int score, int highScore, int lines, int level)
{
    WORD titleAttr   = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    WORD labelAttr   = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    WORD valueAttr   = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    WORD highAttr    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD borderAttr  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    WORD controlAttr = FOREGROUND_GREEN | FOREGROUND_BLUE;

    int y = 1;

    drawString(INFO_X, y, "TETRIS", titleAttr);
    y += 2;

    drawString(INFO_X, y, "Score:", labelAttr);
    char scoreBuf[16];
    snprintf(scoreBuf, sizeof(scoreBuf), " %d", score);
    drawString(INFO_X + 7, y, scoreBuf, valueAttr);
    y += 2;

    drawString(INFO_X, y, "Best:", labelAttr);
    char highBuf[16];
    snprintf(highBuf, sizeof(highBuf), " %d", highScore);
    drawString(INFO_X + 7, y, highBuf, highAttr);
    y += 2;

    drawString(INFO_X, y, "Lines:", labelAttr);
    char linesBuf[16];
    snprintf(linesBuf, sizeof(linesBuf), " %d", lines);
    drawString(INFO_X + 7, y, linesBuf, valueAttr);
    y += 2;

    drawString(INFO_X, y, "Level:", labelAttr);
    char levelBuf[16];
    snprintf(levelBuf, sizeof(levelBuf), " %d", level);
    drawString(INFO_X + 7, y, levelBuf, valueAttr);
    y += 3;

    drawString(INFO_X, y, "[Controls]", borderAttr);
    y += 2;

    drawString(INFO_X, y, "A/D : Move", controlAttr);
    ++y;
    drawString(INFO_X, y, "W   : Rotate", controlAttr);
    ++y;
    drawString(INFO_X, y, "S   : Soft Drop", controlAttr);
    ++y;
    drawString(INFO_X, y, "Space:Hard Drop", controlAttr);
    ++y;
    drawString(INFO_X, y, "P   : Pause", controlAttr);
    ++y;
    drawString(INFO_X, y, "Q   : Quit", controlAttr);
}

void Renderer::render(const Board& board, const Piece& piece,
                      int score, int highScore, int lines, int level)
{
    if (!hConsole) return;

    clearBuffer();
    drawBoard(board);
    drawPiece(piece);
    drawInfo(score, highScore, lines, level);
    flush();
}

void Renderer::renderPaused(const Board& board,
                            int score, int highScore, int lines, int level)
{
    if (!hConsole) return;

    clearBuffer();
    drawBoard(board);
    // Do NOT draw the current piece — hide it during pause
    drawInfo(score, highScore, lines, level);

    // Overlay "PAUSED" text on the board
    WORD pauseAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    drawBoardOverlay("PAUSED", "Press P to resume", pauseAttr);

    flush();
}

void Renderer::renderGameOver(int finalScore, int highScore)
{
    if (!hConsole) return;

    clearBuffer();

    WORD goAttr    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    WORD scoreAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD bestAttr  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD newAttr   = FOREGROUND_RED | FOREGROUND_INTENSITY;
    WORD hintAttr  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    int centerX = BUFFER_WIDTH / 2;
    int centerY = BUFFER_HEIGHT / 2;

    drawString(centerX - 4, centerY - 3, "GAME OVER", goAttr);

    char scoreBuf[32];
    snprintf(scoreBuf, sizeof(scoreBuf), "Final Score: %d", finalScore);
    drawString(centerX - 8, centerY - 1, scoreBuf, scoreAttr);

    char bestBuf[32];
    snprintf(bestBuf, sizeof(bestBuf), "Best Score:  %d", highScore);
    drawString(centerX - 8, centerY, bestBuf, bestAttr);

    if (finalScore >= highScore && finalScore > 0) {
        drawString(centerX - 5, centerY + 1, "NEW RECORD!", newAttr);
    }

    drawString(centerX - 12, centerY + 3, "Press R to Restart", hintAttr);
    drawString(centerX - 10, centerY + 4, "Press Q to Quit", hintAttr);

    flush();
}

void Renderer::flush()
{
    if (!hConsole) return;

    COORD bufferSize = { BUFFER_WIDTH, BUFFER_HEIGHT };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = { 0, 0, BUFFER_WIDTH - 1, BUFFER_HEIGHT - 1 };

    WriteConsoleOutputW(hConsole, &buffer[0][0], bufferSize, bufferCoord, &writeRegion);
}
