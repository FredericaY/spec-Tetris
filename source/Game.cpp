#include "Game.h"
#include <thread>
#include <fstream>
#include <cstdio>

Game::Game()
    : running(false)
    , gameOver(false)
    , paused(false)
    , score(0)
    , highScore(0)
    , lines(0)
    , level(1)
    , fallInterval(calcFallInterval())
    , lastFallTime(Clock::now())
    , pauseStartTime(Clock::now())
    , rng(std::random_device{}())
{
    loadHighScore();
}

Game::~Game()
{
    renderer.shutdown();
}

void Game::loadHighScore()
{
    std::ifstream file(HIGH_SCORE_FILE);
    if (!file.is_open()) {
        highScore = 0;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();

    auto pos = content.find("\"highScore\"");
    if (pos != std::string::npos) {
        auto colon = content.find(':', pos);
        if (colon != std::string::npos) {
            auto numStart = colon + 1;
            while (numStart < content.size() && 
                   (content[numStart] == ' ' || content[numStart] == '\t')) {
                ++numStart;
            }
            highScore = 0;
            while (numStart < content.size() && content[numStart] >= '0' && content[numStart] <= '9') {
                highScore = highScore * 10 + (content[numStart] - '0');
                ++numStart;
            }
        }
    }
}

void Game::saveHighScore()
{
    std::ofstream file(HIGH_SCORE_FILE, std::ios::trunc);
    if (!file.is_open()) {
        return;
    }
    file << "{\n";
    file << "  \"highScore\": " << highScore << "\n";
    file << "}\n";
    file.close();
}

void Game::togglePause()
{
    if (gameOver) return;

    paused = !paused;

    if (paused) {
        // Record when pause started so we can compensate the fall timer
        pauseStartTime = Clock::now();
    } else {
        // Resume: shift lastFallTime forward by the pause duration
        // so the piece doesn't immediately drop
        auto pauseDuration = Clock::now() - pauseStartTime;
        lastFallTime += pauseDuration;
    }
}

void Game::run()
{
    renderer.init();

    running = true;
    gameOver = false;

    if (!spawnPiece()) {
        gameOver = true;
    }

    // Main loop
    while (running) {
        handleInput();

        if (!gameOver && !paused) {
            update();
        }

        render();

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

    // Pause toggle is available in-game and when paused
    if (action == Action::Pause) {
        togglePause();
        return;
    }

    // Restart is available in game-over state
    if (gameOver) {
        if (action == Action::Restart) {
            restart();
        }
        return;
    }

    // When paused, ignore all gameplay actions
    if (paused) {
        return;
    }

    // In-game actions
    switch (action) {
        case Action::MoveLeft: {
            currentPiece.moveLeft();
            if (!board.canPlace(currentPiece)) {
                currentPiece.moveRight();
            }
            break;
        }
        case Action::MoveRight: {
            currentPiece.moveRight();
            if (!board.canPlace(currentPiece)) {
                currentPiece.moveLeft();
            }
            break;
        }
        case Action::MoveDown: {
            currentPiece.moveDown();
            if (!board.canPlace(currentPiece)) {
                currentPiece.moveUp();
                lockPiece();
            } else {
                score += 1;
            }
            break;
        }
        case Action::Rotate: {
            currentPiece.rotate();
            if (!board.canPlace(currentPiece)) {
                bool kicked = false;
                currentPiece.moveLeft();
                if (board.canPlace(currentPiece)) {
                    kicked = true;
                } else {
                    currentPiece.moveRight();
                    currentPiece.moveRight();
                    if (board.canPlace(currentPiece)) {
                        kicked = true;
                    } else {
                        currentPiece.moveLeft();
                    }
                }
                if (!kicked) {
                    currentPiece.rotate();
                    currentPiece.rotate();
                    currentPiece.rotate();
                }
            }
            break;
        }
        case Action::HardDrop: {
            int dropDistance = 0;
            while (true) {
                currentPiece.moveDown();
                if (board.canPlace(currentPiece)) {
                    ++dropDistance;
                } else {
                    currentPiece.moveUp();
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
        currentPiece.moveDown();
        if (!board.canPlace(currentPiece)) {
            currentPiece.moveUp();
            lockPiece();
        }

        lastFallTime = now;
    }
}

void Game::render()
{
    if (gameOver) {
        renderer.renderGameOver(score, highScore);
    } else if (paused) {
        renderer.renderPaused(board, score, highScore, lines, level);
    } else {
        renderer.render(board, currentPiece, score, highScore, lines, level);
    }
}

bool Game::spawnPiece()
{
    std::uniform_int_distribution<int> dist(0, 6);
    PieceType type = static_cast<PieceType>(dist(rng));

    int spawnX = (Board::WIDTH - 4) / 2;
    int spawnY = 0;

    currentPiece.reset(type, spawnX, spawnY);

    if (!board.canPlace(currentPiece)) {
        return false;
    }
    return true;
}

void Game::lockPiece()
{
    board.place(currentPiece);

    int linesCleared = board.clearFullLines();
    if (linesCleared > 0) {
        score += calcScore(linesCleared);
        lines += linesCleared;

        level = calcLevel();
        fallInterval = calcFallInterval();
    }

    if (!spawnPiece()) {
        gameOver = true;
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
    }

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
    paused = false;
    fallInterval = calcFallInterval();
    lastFallTime = Clock::now();
    gameOver = false;

    if (!spawnPiece()) {
        gameOver = true;
    }
}
