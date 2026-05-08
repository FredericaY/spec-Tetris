#ifndef TETRIS_COLOR_H
#define TETRIS_COLOR_H

#include <windows.h>
#include <cstdint>

// Represents the 7 tetromino colors + None for empty cells
enum class Color : uint8_t {
    None    = 0,
    Cyan    = 1,    // I-piece
    Yellow  = 2,    // O-piece
    Magenta = 3,    // T-piece
    Green   = 4,    // S-piece
    Red     = 5,    // Z-piece
    Blue    = 6,    // J-piece
    White   = 7,    // L-piece
};

// Map a Color enum value to Windows Console foreground attribute
// Uses "bright" variants (FOREGROUND_INTENSITY) for visibility on dark backgrounds
inline WORD getConsoleForeground(Color c) {
    switch (c) {
        case Color::Cyan:    return FOREGROUND_GREEN  | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case Color::Yellow:  return FOREGROUND_RED    | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case Color::Magenta: return FOREGROUND_RED    | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case Color::Green:   return FOREGROUND_GREEN  | FOREGROUND_INTENSITY;
        case Color::Red:     return FOREGROUND_RED    | FOREGROUND_INTENSITY;
        case Color::Blue:    return FOREGROUND_BLUE   | FOREGROUND_INTENSITY;
        case Color::White:   return FOREGROUND_RED    | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case Color::None:
        default:             return 0;
    }
}

// Map a Color enum to a Windows Console background attribute
inline WORD getConsoleBackground(Color c) {
    switch (c) {
        case Color::Cyan:    return BACKGROUND_GREEN  | BACKGROUND_BLUE;
        case Color::Yellow:  return BACKGROUND_RED    | BACKGROUND_GREEN;
        case Color::Magenta: return BACKGROUND_RED    | BACKGROUND_BLUE;
        case Color::Green:   return BACKGROUND_GREEN;
        case Color::Red:     return BACKGROUND_RED;
        case Color::Blue:    return BACKGROUND_BLUE;
        case Color::White:   return BACKGROUND_RED    | BACKGROUND_GREEN | BACKGROUND_BLUE;
        case Color::None:
        default:             return 0;
    }
}

#endif // TETRIS_COLOR_H
