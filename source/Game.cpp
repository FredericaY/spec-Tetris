#include "Game.h"
#include <thread>

Game::Game()
    : running(false)
    , gameOver(false)
    , score(0)
    , lines(0)
    , level(1)
    , fallInterval(calcFallInterval())
    , lastFallTime(Clock::now())
    , rng(std::random_device{}())
{
}

Game::~Game()
{
    renderer.shutdown();
}

void Game::run()
{
    renderer.init();

    running = true;
    gameOver = false;

    if (!spawnPiece()) {
        // Spawn collision at start — shouldn't happen on empty board
        gameOver = true;
    }

    // Main loop
    while (running) {
        handleInput();

        if (!gameOver) {
            update();
        }

        render();

        // Frame rate cap (~60 fps)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    renderer.shutdown();
}

void Game::handleInput()
{
    Action action = input.poll();

    if (action == Action::None) {
        return;
    }

    // Quit is always available
    if (action == Action::Quit) {
        running = false;
        return;
    }

    // Restart is available in game-over state
    if (gameOver) {
        if (action == Action::Restart) {
            restart();
        }
        return;
    }

    // In-game actions
    switch (action) {
        case Action::MoveLeft: {
            currentPiece.moveLeft();
            if (!board.canPlace(currentPiece)) {
                currentPiece.moveRight();  // revert
            }
            break;
        }
        case Action::MoveRight: {
            currentPiece.moveRight();
            if (!board.canPlace(currentPiece)) {
                currentPiece.moveLeft();  // revert
            }
            break;
        }
        case Action::MoveDown: {
            // Soft drop: move down one row, add score
            currentPiece.moveDown();
            if (!board.canPlace(currentPiece)) {
                currentPiece.moveUp();  // revert
                lockPiece();
            } else {
                score += 1;  // +1 point per soft-dropped row
            }
            break;
        }
        case Action::Rotate: {
            currentPiece.rotate();
            if (!board.canPlace(currentPiece)) {
                // Simple wall-kick: try moving left or right by 1
                bool kicked = false;
                // Try left
                currentPiece.moveLeft();
                if (board.canPlace(currentPiece)) {
                    kicked = true;
                } else {
                    currentPiece.moveRight(); // revert left
                    // Try right
                    currentPiece.moveRight();
                    if (board.canPlace(currentPiece)) {
                        kicked = true;
                    } else {
                        currentPiece.moveLeft(); // revert right
                    }
                }
                if (!kicked) {
                    // Undo rotation: rotate 3 more times = back to original
                    currentPiece.rotate();
                    currentPiece.rotate();
                    currentPiece.rotate();
                }
            }
            break;
        }
        case Action::HardDrop: {
            // Move piece down until collision, scoring 2 per row
            int dropDistance = 0;
            while (true) {
                currentPiece.moveDown();
                if (board.canPlace(currentPiece)) {
                    ++dropDistance;
                } else {
                    currentPiece.moveUp();  // revert last invalid move
                    break;
                }
            }
            score += dropDistance * 2;
            lockPiece();
            break;
        }
        default:
            break;
    }
}

void Game::update()
{
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFallTime);

    if (elapsed >= fallInterval) {
        // Try to move piece down
        currentPiece.moveDown();
        if (!board.canPlace(currentPiece)) {
            currentPiece.moveUp();  // revert
            lockPiece();
        }

        lastFallTime = now;
    }
}

void Game::render()
{
    if (gameOver) {
        renderer.renderGameOver(score);
    } else {
        renderer.render(board, currentPiece, score, lines, level);
    }
}

bool Game::spawnPiece()
{
    // Random piece type
    std::uniform_int_distribution<int> dist(0, 6);
    PieceType type = static_cast<PieceType>(dist(rng));

    // Spawn position: centered horizontally, at or above the top
    int spawnX = (Board::WIDTH - 4) / 2;  // center 4-wide bounding box
    int spawnY = 0;

    currentPiece.reset(type, spawnX, spawnY);

    if (!board.canPlace(currentPiece)) {
        return false;
    }
    return true;
}

void Game::lockPiece()
{
    // Lock piece into board
    board.place(currentPiece);

    // Clear full lines
    int linesCleared = board.clearFullLines();
    if (linesCleared > 0) {
        score += calcScore(linesCleared);
        lines += linesCleared;

        // Recalculate level and speed
        level = calcLevel();
        fallInterval = calcFallInterval();
    }

    // Try to spawn next piece
    if (!spawnPiece()) {
        gameOver = true;
    }

    // Reset fall timer so new piece doesn't drop immediately
    lastFallTime = Clock::now();
}

int Game::calcScore(int linesCleared) const
{
    switch (linesCleared) {
        case 1:  return 100;
        case 2:  return 300;
        case 3:  return 500;
        case 4:  return 800;
        default: return 0;
    }
}

int Game::calcLevel() const
{
    return lines / 10 + 1;
}

std::chrono::milliseconds Game::calcFallInterval() const
{
    int ms = 800 - (level - 1) * 50;
    if (ms < 100) {
        ms = 100;
    }
    return std::chrono::milliseconds(ms);
}

void Game::restart()
{
    board.clear();
    score = 0;
    lines = 0;
    level = 1;
    fallInterval = calcFallInterval();
    lastFallTime = Clock::now();
    gameOver = false;

    if (!spawnPiece()) {
        // Should never happen on a fresh board
        gameOver = true;
    }
}
