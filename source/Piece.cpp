#include "Piece.h"

// Standard 7 Tetromino shapes (4 rotation states each, stored in 4×4 matrices)
//
// Layout: SHAPES[type][rotation][row][col]
//         row 0 = top, col 0 = left
//         1 = filled cell, 0 = empty
//
// Rotation is clockwise: rotation 1 = rotation 0 rotated 90° CW
// S and Z have only 2 distinct states (0↔1, 2=0, 3=1)
// O has 1 distinct state (all 4 identical)

const int Piece::SHAPES[7][4][4][4] = {
    // I-piece
    {
        { // Rotation 0: horizontal
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 1: vertical
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
        },
        { // Rotation 2: horizontal (same as r0)
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 3: vertical (same as r1)
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
        },
    },
    // O-piece
    {
        { // Rotation 0
            { 1, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 1 (same)
            { 1, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 2 (same)
            { 1, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 3 (same)
            { 1, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    // T-piece
    {
        { // Rotation 0
            { 0, 1, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 1
            { 0, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 2
            { 0, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 3
            { 0, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    // S-piece
    {
        { // Rotation 0
            { 0, 1, 1, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 1
            { 0, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 2 (same as r0)
            { 0, 1, 1, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 3 (same as r1)
            { 0, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 0 },
        },
    },
    // Z-piece
    {
        { // Rotation 0
            { 1, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 1
            { 0, 0, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 2 (same as r0)
            { 1, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 3 (same as r1)
            { 0, 0, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    // J-piece
    {
        { // Rotation 0
            { 1, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 1
            { 0, 1, 1, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 2
            { 0, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 3
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    // L-piece
    {
        { // Rotation 0
            { 0, 0, 1, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 1
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 2
            { 0, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 1, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        { // Rotation 3
            { 1, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
};

Piece::Piece(PieceType t, int startX, int startY)
    : type(t), x(startX), y(startY), rotation(0)
{
    updateShape();
}

void Piece::reset(PieceType t, int startX, int startY)
{
    type = t;
    x = startX;
    y = startY;
    rotation = 0;
    updateShape();
}

void Piece::rotate()
{
    rotation = (rotation + 1) % 4;
    updateShape();
}

void Piece::updateShape()
{
    const int* src = reinterpret_cast<const int*>(SHAPES[static_cast<int>(type)][rotation]);
    int* dst = reinterpret_cast<int*>(shape);
    for (int i = 0; i < 16; ++i) {
        dst[i] = src[i];
    }
}
