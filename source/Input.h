#ifndef TETRIS_INPUT_H
#define TETRIS_INPUT_H

// Game actions that Input::poll() can return
enum class Action {
    None,
    MoveLeft,
    MoveRight,
    MoveDown,
    Rotate,
    HardDrop,
    Pause,
    Quit,
    Restart,     // only available in GameOver state
};

// Non-blocking keyboard input handler for Windows console
class Input {
public:
    Input() = default;

    // Poll keyboard state.
    // Returns the first available action since last poll, or None if no input.
    // Arrow keys and WASD keys are both supported.
    Action poll();

private:
    // Read a keyboard event (raw _getch values) and translate to Action
    Action readKey();

    // Buffer for multi-byte sequences (arrow keys send 0xE0 + code)
    bool awaitingArrow = false;
};

#endif // TETRIS_INPUT_H
