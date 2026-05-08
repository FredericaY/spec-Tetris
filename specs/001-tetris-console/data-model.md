# Data Model: Windows Console Tetris (C++)

**Date**: 2026-05-08 | **Phase**: 1 - Design

## Entity Overview

```
┌──────────────┐       ┌──────────────┐
│    Game      │──────▶│    Board     │
│              │       │  (10×20)     │
│ - score      │       │  grid[][]    │
│ - lines      │       └──────┬───────┘
│ - level      │              │ uses
│ - running    │       ┌──────▼───────┐
└──────┬───────┘       │    Piece     │
       │               │              │
       │ controls      │ - type       │
┌──────▼───────┐       │ - x, y       │
│   Renderer   │       │ - rotation   │
│              │       │ - shape[4][4]│
│ - buffer[][] │       └──────────────┘
└──────────────┘
       ▲
       │ reads from
┌──────┴───────┐
│    Input     │
│              │
│ - poll()     │
└──────────────┘
```

## 1. Board（棋盘）

### 数据结构

```cpp
// Board.h

enum class Color : uint8_t {
    None    = 0,
    Cyan    = 1,    // I
    Yellow  = 2,    // O
    Magenta = 3,    // T
    Green   = 4,    // S
    Red     = 5,    // Z
    Blue    = 6,    // J
    White   = 7,    // L
};

struct Cell {
    bool occupied;   // 是否被占用
    Color color;     // 方块颜色
};

class Board {
    static const int WIDTH = 10;
    static const int HEIGHT = 20;
    
private:
    Cell grid[HEIGHT][WIDTH];
    
public:
    bool canPlace(const Piece& piece) const;
    void place(const Piece& piece);
    int clearFullLines();
    bool isLineFull(int y) const;
    void clearLine(int y);
    void shiftLinesDown(int fromY);
};
```

### 状态转换

```
初始状态:  所有 grid[y][x].occupied = false
          所有 grid[y][x].color = Color::None

place():   遍历 Piece 的 shape[][]，将非零格子在 grid 中标记 occupied=true, color=piece.color

clearLine(y):  将第 y 行所有格子的 occupied 设为 false
              shiftLinesDown(y): 将 y-1 到 0 行整体下移一行
```

## 2. Piece（方块）

### 数据结构

```cpp
// Piece.h

class Piece {
public:
    enum Type { I, O, T, S, Z, J, L };
    
private:
    Type type;
    int x, y;           // 方块在棋盘上的参考坐标（左上角）
    int rotation;       // 当前旋转状态 (0-3)
    int shape[4][4];    // 当前形状的 4x4 矩阵
    
    // 静态数据：每种类型的 4 个旋转状态
    static const int SHAPES[7][4][4][4];
    
    void updateShape(); // 根据 type + rotation 更新 shape[][]
    
public:
    Piece(Type type = I);
    
    void setType(Type t);
    void setPosition(int newX, int newY);
    
    void moveLeft()  { x--; }
    void moveRight() { x++; }
    void moveDown()  { y++; }
    void rotate();   // rotation = (rotation + 1) % 4; updateShape()
    
    int getX() const { return x; }
    int getY() const { return y; }
    Type getType() const { return type; }
    const int* getShape() const; // 返回 shape 指针
};
```

### 形状定义 (SHAPES[7][4][4][4])

```
I:  [1 1 1 1]   [0 0 1 0]   [0 0 0 0]   [0 1 0 0]
    [0 0 0 0]   [0 0 1 0]   [1 1 1 1]   [0 1 0 0]
    [0 0 0 0]   [0 0 1 0]   [0 0 0 0]   [0 1 0 0]
    [0 0 0 0]   [0 0 1 0]   [0 0 0 0]   [0 1 0 0]

O:  [1 1 0 0]   (4个旋转状态完全相同)
    [1 1 0 0]
    [0 0 0 0]
    [0 0 0 0]

T:  [0 1 0 0]   [0 1 0 0]   [0 0 0 0]   [1 0 0 0]
    [1 1 1 0]   [1 1 0 0]   [1 1 1 0]   [1 1 0 0]
    [0 0 0 0]   [0 1 0 0]   [0 1 0 0]   [1 0 0 0]
    [0 0 0 0]   [0 0 0 0]   [0 0 0 0]   [0 0 0 0]

S:  [0 1 1 0]   [0 1 0 0]   (S 只有 2 个旋转状态)
    [1 1 0 0]   [0 1 1 0]
    [0 0 0 0]   [0 0 1 0]
    [0 0 0 0]   [0 0 0 0]
    (旋转 2 = 旋转 0, 旋转 3 = 旋转 1)

Z:  [1 1 0 0]   [0 0 1 0]   (Z 只有 2 个旋转状态)
    [0 1 1 0]   [0 1 1 0]
    [0 0 0 0]   [0 1 0 0]
    [0 0 0 0]   [0 0 0 0]

J:  [1 0 0 0]   [1 1 0 0]   [0 0 0 0]   [0 1 0 0]
    [1 1 1 0]   [1 0 0 0]   [1 1 1 0]   [0 1 0 0]
    [0 0 0 0]   [1 0 0 0]   [0 0 1 0]   [1 1 0 0]
    [0 0 0 0]   [0 0 0 0]   [0 0 0 0]   [0 0 0 0]

L:  [0 0 1 0]   [1 0 0 0]   [0 0 0 0]   [1 1 0 0]
    [1 1 1 0]   [1 0 0 0]   [1 1 1 0]   [0 1 0 0]
    [0 0 0 0]   [1 1 0 0]   [1 0 0 0]   [0 1 0 0]
    [0 0 0 0]   [0 0 0 0]   [0 0 0 0]   [0 0 0 0]
```

### 方块颜色映射

```cpp
Color getColorForType(Piece::Type t) {
    switch (t) {
        case Piece::I: return Color::Cyan;
        case Piece::O: return Color::Yellow;
        case Piece::T: return Color::Magenta;
        case Piece::S: return Color::Green;
        case Piece::Z: return Color::Red;
        case Piece::J: return Color::Blue;
        case Piece::L: return Color::White;
        default: return Color::None;
    }
}
```

## 3. Game State

### 状态机

```
┌───────────┐    启动     ┌───────────┐
│  (Start)  │───────────▶│  Running  │
└───────────┘            └─────┬─────┘
                               │
                   新方块碰撞   │
                               ▼
                         ┌───────────┐
                         │ Game Over │
                         └─────┬─────┘
                               │ 按键 (R)
                               ▼
                         ┌───────────┐
                         │  Running  │ (新游戏)
                         └───────────┘
```

### 游戏数据

| 字段 | 类型 | 初始值 | 说明 |
|------|------|--------|------|
| `score` | int | 0 | 当前分数 |
| `lines` | int | 0 | 已消除总行数 |
| `level` | int | 1 | 当前等级 |
| `running` | bool | true | 游戏运行状态 |
| `fallInterval` | std::chrono::milliseconds | 800ms | 自动下落间隔 |

### 计分规则

| 消除行数 | 分数 |
|----------|------|
| 1 行 | 100 |
| 2 行 | 300 |
| 3 行 | 500 |
| 4 行 | 800 |
| 硬降每格 | +2 |

### 等级与速度

| 等级阈值 | 累积消除行数 | 下落间隔 |
|----------|-------------|----------|
| Level 1 | 0-9 | 800ms |
| Level 2 | 10-19 | 750ms |
| Level 3 | 20-29 | 700ms |
| ... | ... | ... |
| Level N | (N-1)*10 ~ N*10-1 | max(100ms, 800 - (N-1)*50) |

## 4. Renderer Buffer

### CHAR_INFO 缓冲区

```cpp
// 渲染区域: 棋盘(10x20) + 侧边信息面板(12 列)
// 总缓冲区大小: 34 x 22 (含边框)

static const int BUFFER_WIDTH = 34;   // 10 棋盘 + 2 边框 + 12 信息面板 + 边距
static const int BUFFER_HEIGHT = 22;  // 20 行棋盘 + 2 行边框

CHAR_INFO buffer[BUFFER_HEIGHT][BUFFER_WIDTH];
```

**布局**:

```
  0         1         2         3
  0123456789012345678901234567890123
0 +----------+  ┌──────────────────┐
1 |..........|  │  Tetris           │
2 |..........|  │                   │
3 |..........|  │  Score: 0         │
4 |..........|  │  Lines: 0         │
5 |..........|  │  Level: 1         │
6 |..........|  │                   │
7 |..........|  │  Controls:        │
  ...            │  ←→ A/D: Move   │
20|..........|  │  ↑ W: Rotate     │
21+----------+  │  ↓ S: Soft Drop  │
                │  Space: Hard Drop│
                │  Q/Esc: Quit     │
                └──────────────────┘
```

**说明**: 0-11 列为棋盘区域（10 列 + 2 列边框），13-33 列为信息面板。

## 5. Input Mapping

```cpp
enum class Action {
    None,
    MoveLeft,
    MoveRight,
    MoveDown,
    Rotate,
    HardDrop,
    Quit,
    Restart     // 游戏结束后重新开始
};

// WASD 映射:
// A / 左箭头 → MoveLeft
// D / 右箭头 → MoveRight
// S / 下箭头 → MoveDown
// W / 上箭头 → Rotate
// 空格       → HardDrop
// Q / Esc   → Quit
// R         → Restart (仅游戏结束时)
```
