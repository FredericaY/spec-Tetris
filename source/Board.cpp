#include "Board.h"

Board::Board()
{
    clear();
}

void Board::clear()
{
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            grid[y][x].occupied = false;
            grid[y][x].color = Color::None;
        }
    }
}

bool Board::isOccupied(int x, int y) const
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return true;  // out-of-bounds = occupied (can't place)
    }
    return grid[y][x].occupied;
}

Color Board::getColor(int x, int y) const
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return Color::None;
    }
    return grid[y][x].color;
}

bool Board::canPlace(const Piece& piece) const
{
    int px = piece.getX();
    int py = piece.getY();

    for (int r = 0; r < 4; ++r) {
        for (int col = 0; col < 4; ++col) {
            if (piece.isFilled(r, col)) {
                int bx = px + col;
                int by = py + r;

                // Check horizontal boundaries
                if (bx < 0 || bx >= WIDTH) {
                    return false;
                }
                // Check bottom boundary
                if (by >= HEIGHT) {
                    return false;
                }
                // Allow above-board positions (by < 0) for spawning
                if (by < 0) {
                    continue;
                }
                // Check collision with existing blocks
                if (grid[by][bx].occupied) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Board::place(const Piece& piece)
{
    int px = piece.getX();
    int py = piece.getY();
    Color pieceColor = piece.getColor();

    for (int r = 0; r < 4; ++r) {
        for (int col = 0; col < 4; ++col) {
            if (piece.isFilled(r, col)) {
                int bx = px + col;
                int by = py + r;

                if (bx >= 0 && bx < WIDTH && by >= 0 && by < HEIGHT) {
                    grid[by][bx].occupied = true;
                    grid[by][bx].color = pieceColor;
                }
            }
        }
    }
}

int Board::clearFullLines()
{
    int cleared = 0;

    // Scan from bottom to top
    for (int y = HEIGHT - 1; y >= 0; --y) {
        if (isLineFull(y)) {
            shiftLinesDown(y);
            ++cleared;
            // After shifting, row y now contains what was row y-1.
            // Re-check this same row index (++y cancels the loop --y).

            // row y now has old row y-1 content, row 0 is empty.
            // We keep y the same and re-check.

            ++y;
        }
    }
    return cleared;
}

bool Board::isLineFull(int y) const
{
    for (int x = 0; x < WIDTH; ++x) {
        if (!grid[y][x].occupied) {
            return false;
        }
    }
    return true;
}

void Board::clearLine(int y)
{
    for (int x = 0; x < WIDTH; ++x) {
        grid[y][x].occupied = false;
        grid[y][x].color = Color::None;
    }
}

void Board::shiftLinesDown(int fromY)
{
    // Shift rows [0..fromY-1] down by one row
    // Row fromY is cleared separately by clearLine()
    for (int y = fromY - 1; y >= 0; --y) {
        for (int x = 0; x < WIDTH; ++x) {
            grid[y + 1][x] = grid[y][x];
        }
    }
    // Clear the top row
    for (int x = 0; x < WIDTH; ++x) {
        grid[0][x].occupied = false;
        grid[0][x].color = Color::None;
    }
}
