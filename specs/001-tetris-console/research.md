# Research: Windows Console Tetris (C++)

**Date**: 2026-05-08 | **Phase**: 0 - Technical Research

## 1. Windows Console API 渲染方案

### 候选方案

| 方案 | 描述 | 优点 | 缺点 |
|------|------|------|------|
| A: `std::cout` + ANSI | 使用 ANSI 转义序列控制颜色/光标 | 跨平台 | Windows 10 以下版本不完整支持，PowerShell 中不稳定 |
| B: `SetConsoleCursorPosition` + `WriteConsole` | 逐行输出，用 Windows API 定位光标 | 简单直接 | 可能闪烁，需手动管理光标 |
| C: `WriteConsoleOutput` | 整块写入 CHAR_INFO 缓冲区 | 无闪烁，性能最优，支持颜色属性 | Windows 专用 |

### 决策: 方案 C — `WriteConsoleOutput`

**理由**: 
- 构建 `CHAR_INFO[HEIGHT][WIDTH]` 缓冲区，每帧一次性写入，**完全消除闪烁**
- `CHAR_INFO` 自带 `wAttributes` 字段，直接支持前景色/背景色（无需 ANSI）
- 是 Windows 控制台游戏开发的工业标准方案
- 性能完全满足需求（10x20 棋盘 + 侧栏，约 1200 个字符块）

```cpp
// 核心 API
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
COORD bufferSize = {WIDTH, HEIGHT};
COORD bufferCoord = {0, 0};
SMALL_RECT writeRegion = {0, 0, WIDTH-1, HEIGHT-1};
WriteConsoleOutput(hConsole, buffer, bufferSize, bufferCoord, &writeRegion);
```

## 2. 键盘输入方案

### 候选方案

| 方案 | API | 阻塞 | 功能键支持 |
|------|-----|------|-----------|
| A: `std::cin` | iostream | 阻塞 | 不支持方向键 |
| B: `_kbhit()` + `_getch()` | `<conio.h>` | 非阻塞 | 支持（两字节序列） |
| C: `ReadConsoleInput` | Windows API | 非阻塞 | 完整支持 |
| D: `GetAsyncKeyState` | Windows API | 非阻塞（状态查询） | 支持 |

### 决策: 方案 B — `_kbhit()` + `_getch()`

**理由**:
- **最简单**，无需处理复杂的 `INPUT_RECORD` 结构
- 非阻塞：`_kbhit()` 检查是否有输入，`_getch()` 读取按键
- 方向键作为双字节序列处理（首字节 `0xE0` 或 `0x00`）
- 适合游戏场景：每帧 poll 一次输入即可

```cpp
// 方向键映射
// 上箭头: 0xE0 + 0x48
// 下箭头: 0xE0 + 0x50
// 左箭头: 0xE0 + 0x4B
// 右箭头: 0xE0 + 0x4D
// 或使用虚拟键码: VK_UP=72, VK_DOWN=80, VK_LEFT=75, VK_RIGHT=77
```

**备选**: 同时支持 WASD 作为方向控制（和方向键并列）。

## 3. 编码兼容性方案

### 问题

Windows PowerShell 默认代码页为 GBK (936)，Windows Terminal 默认 UTF-8 (65001)。不同终端环境下中文字符和特殊字符可能乱码。

### 决策: ASCII 优先 + 启动时设置编码

```cpp
// 方案：启动时设为 UTF-8，优先使用 ASCII 绘制
SetConsoleOutputCP(CP_UTF8);  // 65001
SetConsoleCP(CP_UTF8);        // 输入编码
```

**绘制策略**:
- 棋盘方块：`'[' + '#' + ']'` 或 `'#' * 2`，使用 `CHAR_INFO` 颜色属性区分
- 棋盘边框：`'+'`, `'-'`, `'|'` 等 ASCII 线条
- 文字提示：全部使用英文（如 "Score: 0", "Lines: 0", "Level: 1", "GAME OVER"）
- 避免 emoji (⬛⬜🟥)，避免 Unicode 方块字符 (█▄▀■□)，避免特殊边框 (╔═╗)

## 4. 颜色方案

### 方块颜色分配

使用 Windows Console 预定义的 16 色（`FOREGROUND_*` + `BACKGROUND_*`）：

| 方块 | Windows Console 颜色 |
|------|---------------------|
| I | Cyan (FOREGROUND_GREEN \| FOREGROUND_BLUE) |
| O | Yellow (FOREGROUND_RED \| FOREGROUND_GREEN) |
| T | Magenta (FOREGROUND_RED \| FOREGROUND_BLUE) |
| S | Green (FOREGROUND_GREEN) |
| Z | Red (FOREGROUND_RED) |
| J | Blue (FOREGROUND_BLUE) |
| L | White/Bright (FOREGROUND_RED \| FOREGROUND_GREEN \| FOREGROUND_BLUE) |

**渲染策略**: 
- 方块单元格：用 `'#'` 或其他 ASCII 字符，背景色 = 方块颜色，前景色 = 方块颜色
- 更佳方案：每个方块单元格使用两个字符（如 `"[]"`），前景色 = 方块颜色，背景色 = 黑色，更清晰
- 固定方块和当前方块使用相同颜色，通过位置区分
- 空白区域：空格 + 黑色背景

**最终选择**: 双字符 `"[]"` 方案 — 前景色 = 方块颜色，背景色 = 黑色。这样在 PowerShell 中视觉效果最好，且无歧义。

## 5. 游戏循环与时间控制

### 方案

使用 `std::chrono` 实现帧率独立的下落计时器：

```cpp
using Clock = std::chrono::steady_clock;
auto lastFallTime = Clock::now();
auto fallInterval = std::chrono::milliseconds(800); // 初始 800ms

while (running) {
    auto now = Clock::now();
    auto dt = now - lastFallTime;
    
    if (dt >= fallInterval) {
        piece.moveDown();
        if (collision) { lockPiece(); }
        lastFallTime = now;
    }
    
    handleInput();
    render();
    Sleep(16); // ~60fps cap
}
```

**下落间隔**: 初始 800ms，每消除 10 行升一级，间隔减少 50ms，最低 100ms。

## 6. 方块旋转系统

### 决策: 简化 SRS（不考虑墙踢）

- 每种方块存储 4 个旋转状态的 4x4 矩阵
- 旋转时：尝试旋转 → 碰撞检测 → 碰撞则阻止
- 不做墙踢（wall kick），保持简单
- 后续 spec 可增强为完整 SRS

## 7. 硬降实现

硬降（空格键）：循环将方块向下移动，每次移动后检测碰撞，碰撞时停止并锁定。

```cpp
void hardDrop() {
    while (board.canPlace(piece.movedDown())) {
        piece.moveDown();
        score += 2; // 每下降一格加 2 分
    }
    lockPiece();
}
```

## 8. 编译工具

### 选择: CMake + MSVC 或 MinGW-w64

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(TetrisConsole VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(tetris source/main.cpp source/Game.cpp source/Board.cpp 
               source/Piece.cpp source/Renderer.cpp source/Input.cpp)
```

**MinGW 简单编译**:
```bash
g++ -std=c++17 -O2 source/*.cpp -o tetris.exe
```

## 总结

| 技术决策 | 选择 |
|----------|------|
| 渲染方案 | `WriteConsoleOutput`（双缓冲，零闪烁） |
| 输入方案 | `_kbhit()` + `_getch()`（非阻塞） |
| 颜色方案 | Windows Console 16 色 + `CHAR_INFO` 属性 |
| 时间控制 | `std::chrono::steady_clock` |
| 编码处理 | `SetConsoleOutputCP(CP_UTF8)` + ASCII 优先绘制 |
| 方块显示 | 双字符 `"[]"`，前景色 = 方块颜色 |
| 旋转系统 | 简化 SRS（碰撞则阻止，无墙踢） |
| 编译工具 | CMake + MSVC 或 MinGW-w64 g++ |
