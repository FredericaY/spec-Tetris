#ifndef TETRIS_BOARD_H
#define TETRIS_BOARD_H

#include "Color.h"
#include "Piece.h"

// A single cell on the board
struct Cell {
    bool occupied = false;
    Color color = Color::None;
};

// The game board: a grid of 10 columns × 20 rows
class Board {
public:
    static constexpr int WIDTH  = 10;
    static constexpr int HEIGHT = 20;

    Board();

    // Reset board to empty state
    void clear();

    // Check if a piece can be placed at its current position
    // (no overlap with occupied cells and within bounds)
    bool canPlace(const Piece& piece) const;

    // Lock a piece into the board (marks cells as occupied with piece's color)
    void place(const Piece& piece);

    // Clear all full lines (from bottom up), shift down above content.
    // Returns number of lines cleared.
    int clearFullLines();

    // Access individual cells
    bool isOccupied(int x, int y) const;
    Color getColor(int x, int y) const;

private:
    Cell grid[HEIGHT][WIDTH];

    bool isLineFull(int y) const;
    void clearLine(int y);
    void shiftLinesDown(int fromY);  // shift rows [0..fromY-1] down by 1
};

#endif // TETRIS_BOARD_H
