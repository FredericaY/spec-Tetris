#include "Renderer.h"
#include "Color.h"
#include <cstdio>

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
    // Get the standard output handle
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        hConsole = nullptr;
        return;
    }

    // Set console encoding to UTF-8 for consistent character display
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Hide the blinking cursor during gameplay
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // Set console title
    SetConsoleTitleW(L"Tetris");

    // Set console screen buffer size larger than viewport to prevent scroll bars
    COORD bufferSize = { BUFFER_WIDTH, BUFFER_HEIGHT + 1 };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // Set the console window size
    SMALL_RECT windowRect = { 0, 0, BUFFER_WIDTH - 1, BUFFER_HEIGHT + 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowRect);

    // Initialize the internal buffer
    clearBuffer();
}

void Renderer::shutdown()
{
    if (hConsole) {
        // Restore cursor
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        // Move cursor below the game screen
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

void Renderer::drawBoard(const Board& board)
{
    // Top border
    setCell(BOARD_X, BOARD_Y, L'+', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);
    for (int x = 0; x < Board::WIDTH; ++x) {
        setCell(CELL_X + x, BOARD_Y, L'-', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);
    }
    setCell(BOARD_RIGHT_X, BOARD_Y, L'+', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);

    // Side borders + board cells
    for (int by = 0; by < Board::HEIGHT; ++by) {
        int screenY = BOARD_Y + 1 + by;
        // Left border
        setCell(BOARD_X, screenY, L'|', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);
        // Right border
        setCell(BOARD_RIGHT_X, screenY, L'|', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);

        // Board cells
        for (int bx = 0; bx < Board::WIDTH; ++bx) {
            if (board.isOccupied(bx, by)) {
                Color c = board.getColor(bx, by);
                WORD fg = getConsoleForeground(c);
                setCell(CELL_X + bx, screenY, L'#', fg);
            } else {
                // Empty cell 鈥?keep as space with no color attribute
                setCell(CELL_X + bx, screenY, L' ', 0);
            }
        }
    }

    // Bottom border
    int bottomY = BOARD_Y + 1 + Board::HEIGHT;
    setCell(BOARD_X, bottomY, L'+', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);
    for (int x = 0; x < Board::WIDTH; ++x) {
        setCell(CELL_X + x, bottomY, L'-', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);
    }
    setCell(BOARD_RIGHT_X, bottomY, L'+', (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_INTENSITY);
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

                // Only draw if within visible board bounds
                if (bx >= 0 && bx < Board::WIDTH && by >= 0 && by < Board::HEIGHT) {
                    int screenX = CELL_X + bx;
                    int screenY = BOARD_Y + 1 + by;
                    setCell(screenX, screenY, L'#', fg);
                }
            }
        }
    }
}

void Renderer::drawInfo(int score, int lines, int level)
{
    WORD titleAttr  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // bright white
    WORD labelAttr  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;                        // gray
    WORD valueAttr  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // bright white
    WORD borderAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    WORD controlAttr = FOREGROUND_GREEN | FOREGROUND_BLUE;  // dim cyan for controls

    int y = 1;

    // Title
    drawString(INFO_X, y, "TETRIS", titleAttr);
    y += 2;

    // Score
    drawString(INFO_X, y, "Score:", labelAttr);
    char scoreBuf[16];
    snprintf(scoreBuf, sizeof(scoreBuf), " %d", score);
    drawString(INFO_X + 7, y, scoreBuf, valueAttr);
    y += 2;

    // Lines
    drawString(INFO_X, y, "Lines:", labelAttr);
    char linesBuf[16];
    snprintf(linesBuf, sizeof(linesBuf), " %d", lines);
    drawString(INFO_X + 7, y, linesBuf, valueAttr);
    y += 2;

    // Level
    drawString(INFO_X, y, "Level:", labelAttr);
    char levelBuf[16];
    snprintf(levelBuf, sizeof(levelBuf), " %d", level);
    drawString(INFO_X + 7, y, levelBuf, valueAttr);
    y += 3;

    // Controls header
    drawString(INFO_X, y, "[Controls]", borderAttr);
    y += 2;

    // Control mappings
    drawString(INFO_X, y, "A/D : Move", controlAttr);
    ++y;
    drawString(INFO_X, y, "W   : Rotate", controlAttr);
    ++y;
    drawString(INFO_X, y, "S   : Soft Drop", controlAttr);
    ++y;
    drawString(INFO_X, y, "Space:Hard Drop", controlAttr);
    ++y;
    drawString(INFO_X, y, "Q   : Quit", controlAttr);
}

void Renderer::render(const Board& board, const Piece& piece,
                      int score, int lines, int level)
{
    if (!hConsole) return;

    clearBuffer();
    drawBoard(board);
    drawPiece(piece);
    drawInfo(score, lines, level);
    flush();
}

void Renderer::renderGameOver(int finalScore)
{
    if (!hConsole) return;

    clearBuffer();

    WORD goAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // bright white
    WORD scoreAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // bright yellow
    WORD hintAttr  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;     // gray

    int centerX = BUFFER_WIDTH / 2;
    int centerY = BUFFER_HEIGHT / 2;

    drawString(centerX - 4, centerY - 2, "GAME OVER", goAttr);

    char scoreBuf[32];
    snprintf(scoreBuf, sizeof(scoreBuf), "Final Score: %d", finalScore);
    drawString(centerX - 8, centerY, scoreBuf, scoreAttr);

    drawString(centerX - 12, centerY + 2, "Press R to Restart", hintAttr);
    drawString(centerX - 10, centerY + 3, "Press Q to Quit", hintAttr);

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
