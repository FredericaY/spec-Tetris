# Quickstart: Windows Console Tetris (C++)

**Date**: 2026-05-08

## 环境要求

- **操作系统**: Windows 10 或 Windows 11
- **编译器**: 
  - MSVC (Visual Studio 2019/2022 Build Tools) **或**
  - MinGW-w64 (g++ 9.0+, 推荐 MSYS2)
- **终端**: Windows PowerShell 或 Windows Terminal
- **CMake**: 3.16+（可选，也可直接用命令行编译）

## 快速编译与运行

### 方式 1: MSVC (Visual Studio 命令行)

```bash
# 在 Visual Studio Developer Command Prompt 中:
cl /std:c++17 /EHsc /O2 /Fe:tetris.exe source\*.cpp
tetris.exe
```

### 方式 2: MinGW-w64 (g++)

```bash
# 推荐使用 MSYS2 中的 MinGW-w64:
g++ -std=c++17 -O2 source/*.cpp -o tetris.exe -static
.\tetris.exe
```

### 方式 3: CMake

```bash
mkdir build && cd build

# MSVC:
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\tetris.exe

# MinGW:
cmake .. -G "MinGW Makefiles"
cmake --build .
.\tetris.exe
```

## 运行验证

启动后应看到：

```
┌─────────────────────────────────┐
│ +----------+  ┌──────────────┐  │
│ |          |  │  Tetris       │  │
│ |          |  │               │  │
│ |          |  │  Score: 0     │  │
│ |          |  │  Lines: 0     │  │
│ |    []   |  │  Level: 1      │  │
│ |          |  │               │  │
│ |          |  │  ←→ A/D: Move │  │
│ |          |  │  ↑ W: Rotate  │  │
│ |          |  │  ↓ S: Drop    │  │
│ |          |  │  Space: Hard  │  │
│ |          |  │  Q/Esc: Quit  │  │
│ +----------+  └──────────────┘  │
└─────────────────────────────────┘
```

方块从顶部下落，每 ~800ms 自动下降一格。

## 操作说明

| 按键 | 操作 |
|------|------|
| A / ← | 左移 |
| D / → | 右移 |
| S / ↓ | 软降（加速下落） |
| W / ↑ | 旋转 |
| 空格 | 硬降（直接落底） |
| Q / Esc | 退出游戏 |
| R | 重新开始（游戏结束后） |

## 目录结构

```
tetris-console/
├── source/
│   ├── main.cpp          # 入口
│   ├── Game.h / .cpp     # 主循环 & 状态管理
│   ├── Board.h / .cpp    # 棋盘 & 碰撞检测
│   ├── Piece.h / .cpp    # 方块 & 旋转
│   ├── Renderer.h / .cpp # 控制台渲染
│   ├── Input.h / .cpp    # 键盘输入
│   └── Colors.h          # 颜色常量
├── CMakeLists.txt
├── specs/001-tetris-console/
│   ├── spec.md           # 功能规格
│   ├── plan.md           # 实现计划
│   ├── research.md       # 技术研究
│   ├── data-model.md     # 数据模型
│   ├── quickstart.md     # 本文
│   └── tasks.md          # 任务拆分
└── README.md
```
