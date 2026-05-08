#ifndef TETRIS_PIECE_H
#define TETRIS_PIECE_H

#include "Color.h"
#include <cstdint>

// Classic 7 Tetromino types
enum class PieceType : uint8_t {
    I = 0,
    O = 1,
    T = 2,
    S = 3,
    Z = 4,
    J = 5,
    L = 6,
};

// Map PieceType → Color
inline Color getColorForPieceType(PieceType type) {
    switch (type) {
        case PieceType::I: return Color::Cyan;
        case PieceType::O: return Color::Yellow;
        case PieceType::T: return Color::Magenta;
        case PieceType::S: return Color::Green;
        case PieceType::Z: return Color::Red;
        case PieceType::J: return Color::Blue;
        case PieceType::L: return Color::White;
        default:           return Color::None;
    }
}

class Piece {
public:
    // All 7 tetromino shapes, each with 4 rotation states (4x4 grid)
    // SHAPES[type][rotation][row][col]
    // 1 = filled, 0 = empty
    static const int SHAPES[7][4][4][4];

    // Construct a piece of the given type at initial position
    explicit Piece(PieceType t = PieceType::I, int startX = 3, int startY = 0);

    // Reset piece with new type and position
    void reset(PieceType t, int startX, int startY);

    // Movement (no collision check — caller must verify)
    void moveLeft()  { --x; }
    void moveRight() { ++x; }
    void moveDown()  { ++y; }
    void moveUp()    { --y; }

    // Piece position represents top-left corner of 4x4 bounding box on the board
    int getX() const { return x; }
    int getY() const { return y; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }

    // Rotation
    void rotate();              // clockwise: (rotation + 1) % 4
    int getRotation() const { return rotation; }

    // Type and color
    PieceType getType() const { return type; }
    Color getColor() const { return getColorForPieceType(type); }

    // Access current shape: returns pointer to 4x4 int array (row-major)
    const int* getShape() const { return reinterpret_cast<const int*>(shape); }

    // Test if a specific cell in the 4x4 shape matrix is filled
    bool isFilled(int r, int c) const { return shape[r][c] != 0; }

private:
    PieceType type;
    int x, y;                    // top-left position on the board
    int rotation;                // 0-3
    int shape[4][4];             // current shape from SHAPES[type][rotation]

    void updateShape();          // copy SHAPES[type][rotation] → shape[][]
};

#endif // TETRIS_PIECE_H
