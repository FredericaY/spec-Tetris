#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "Board.h"
#include "Piece.h"
#include "Renderer.h"
#include "Input.h"
#include <chrono>
#include <random>

class Game {
public:
    Game();
    ~Game();

    // Start the main game loop
    void run();

private:
    // Game state
    bool running;
    bool gameOver;
    int score;
    int lines;
    int level;

    // Timing
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    std::chrono::milliseconds fallInterval;
    TimePoint lastFallTime;

    // Core modules
    Board board;
    Piece currentPiece;
    Renderer renderer;
    Input input;

    // Random piece generation
    std::mt19937 rng;

    // Process player input
    void handleInput();

    // Update game logic (drop timer, etc.)
    void update();

    // Render the current frame
    void render();

    // Spawn a new piece at the top
    // Returns false if spawn position is blocked (game over)
    bool spawnPiece();

    // Lock current piece into board, then:
    // - Add hard drop bonus
    // - Clear full lines and update score
    // - Spawn next piece
    void lockPiece();

    // Calculate score for cleared lines
    int calcScore(int linesCleared) const;

    // Calculate level from total lines cleared
    int calcLevel() const;

    // Calculate fall interval from level
    std::chrono::milliseconds calcFallInterval() const;

    // Reset all state for a new game
    void restart();
};

#endif // TETRIS_GAME_H
