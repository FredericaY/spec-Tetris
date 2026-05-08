#include "Input.h"
#include <conio.h>

Action Input::poll()
{
    if (_kbhit()) {
        return readKey();
    }
    return Action::None;
}

Action Input::readKey()
{
    int ch = _getch();

    // Check for extended key prefix (arrow keys use 0xE0 prefix)
    if (ch == 0xE0 || ch == 0x00) {
        ch = _getch();
        switch (ch) {
            case 75:  return Action::MoveLeft;   // Left arrow
            case 77:  return Action::MoveRight;  // Right arrow
            case 80:  return Action::MoveDown;   // Down arrow
            case 72:  return Action::Rotate;     // Up arrow
            default:  return Action::None;
        }
    }

    // WASD keys
    switch (ch) {
        case 'a': case 'A':  return Action::MoveLeft;
        case 'd': case 'D':  return Action::MoveRight;
        case 's': case 'S':  return Action::MoveDown;
        case 'w': case 'W':  return Action::Rotate;
        case ' ':            return Action::HardDrop;   // Space
        case 'r': case 'R':  return Action::Restart;
        case 'q': case 'Q':  return Action::Quit;
        case 27:             return Action::Quit;       // Escape
        default:             return Action::None;
    }
}
