# Tasks: Windows Console Tetris (C++)

**Input**: Design documents from `/specs/001-tetris-console/`
**Prerequisites**: plan.md (✓), spec.md (✓), research.md (✓), data-model.md (✓), quickstart.md (✓)

**Organization**: Tasks are grouped by user story to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2)
- Include exact file paths in descriptions

## Path Conventions

- Source code: `source/` at repository root
- Spec docs: `specs/001-tetris-console/`

---

## Phase 1: Setup (Project Infrastructure)

**Purpose**: Create project structure and build configuration before any code

- [ ] T001 Create `source/` directory and CMakeLists.txt with C++17 standard, Windows subsystem, linking requirements
- [ ] T002 [P] Create `source/Color.h` with Color enum, color-to-Windows-attribute mapping function, getColorForType(Piece::Type)
- [ ] T003 [P] Create `source/Input.h` with Action enum (None/MoveLeft/MoveRight/MoveDown/Rotate/HardDrop/Quit/Restart) and Input class declaration

**Checkpoint**: Project compiles with empty main() — build system verified

---

## Phase 2: Foundational — Board + Piece Data Layer

**Purpose**: Core data structures that ALL user stories depend on

**⚠️ CRITICAL**: No gameplay tasks can start until Board and Piece are implemented

- [ ] T004 [P] Create `source/Board.h` with Board class declaration: WIDTH=10, HEIGHT=20 constants, Cell struct, grid[HEIGHT][WIDTH], canPlace(), place(), clearFullLines(), isLineFull(), clearLine(), shiftLinesDown() method signatures
- [ ] T005 [P] Create `source/Piece.h` with Piece class declaration: Type enum (I/O/T/S/Z/J/L), shape[4][4], x/y/rotation fields, moveLeft/Right/Down(), rotate(), static SHAPES[7][4][4][4] declaration
- [ ] T006 [US4] Implement `source/Piece.cpp`: all 7 tetromino shapes (4 rotation states each) in SHAPES constant, updateShape(), move*() methods, rotate() rotation increment logic
- [ ] T007 Implement `source/Board.cpp`: canPlace() collision detection (bounds + overlap check), place() cell locking, clearFullLines() with single/multi-line logic, shiftLinesDown(), isLineFull()

**Checkpoint**: Board and Piece classes can be instantiated and tested independently (e.g., place piece, check collision, clear lines via unit test or test stub)

---

## Phase 3: User Story 1 — Core Gameplay Loop (Priority: P1) 🎯 MVP

**Goal**: Playable Tetris — block falls, player controls it, lines clear, game ends

**Independent Test**: Launch game → block auto-falls → move/rotate with keys → block locks → full lines clear → stack to top → game over message

### Implementation for US1

- [ ] T008 [P] [US1] Implement `source/Input.cpp`: poll() using _kbhit()/_getch(), handle arrow key two-byte sequences (0xE0 prefix), WASD fallback mapping, Space/Enter/Esc/Q handling, return Action enum
- [ ] T009 [P] [US1] Implement `source/Renderer.h`: class declaration with init(console handle), render(Board, Piece, score/lines/level), renderGameOver(score), shutdown(); CHAR_INFO buffer constants BUFFER_WIDTH=34, BUFFER_HEIGHT=22
- [ ] T010 [P] [US1] Implement `source/Colors.h` Windows console color helpers: setColor16(WORD), a helper to map Color enum → FOREGROUND_* WORD attributes for CHAR_INFO
- [ ] T011 [US1] Implement `source/Renderer.cpp` — basic rendering (no colors yet):
  - init(): GetStdHandle, SetConsoleOutputCP(CP_UTF8), hide cursor, set buffer size
  - render(): Build CHAR_INFO buffer — draw board border ('+', '-', '|'), render board cells (empty='  ' with no fill, occupied='[]' or '##'), render current piece (same char, later with color), draw side panel text (Score/Lines/Level/Controls) using WriteConsoleOutput
  - renderGameOver(): clear screen, show "GAME OVER" + final score
  - shutdown(): show cursor
- [ ] T012 [US1] Create `source/Game.h`: class declaration with run(), handleInput(), update(), render(), spawnPiece(), lockPiece(), clearLines(); fields for Board, currentPiece, Renderer, Input, score/lines/level/running/nextType
- [ ] T013 [US1] Implement `source/Game.cpp` — core game logic:
  - Constructor: init renderer, set starting state
  - spawnPiece(): create new Piece at (WIDTH/2-2, 0), check canPlace() → if collision → game over
  - handleInput(): poll Input → switch on Action → try move/rotate/drop, verify with board.canPlace() before committing
  - update(dt): accumulate fall timer via std::chrono::steady_clock, when elapsed > fallInterval → piece.moveDown() → if !canPlace() → revert moveDown, lockPiece()
  - lockPiece(): board.place(currentPiece), hardDrop scoring, spawnPiece()
  - clearLines(): call board.clearFullLines(), update score/lines, recalc level
  - run(): while(running) { handleInput(); update(deltaTime); render(); Sleep(16); }
- [ ] T014 [US1] Create `source/main.cpp`:
  - SetConsoleTitle("Tetris")
  - Instantiate Game game;
  - Call game.run();
  - Return 0;
  - (~20 lines max)

**Checkpoint**: Full playable Tetris — blocks fall, player controls them, lines clear, score updates, game ends. Currently monochrome (no colors yet).

---

## Phase 4: User Story 2 + 3 — Color Display & Encoding Compatibility (Priority: P1) 🎨

**Goal**: Different block types show different colors; display is stable in both PowerShell (GBK) and Windows Terminal (UTF-8)

**Independent Test**: Launch in PowerShell (GBK code page 936) and Windows Terminal (UTF-8) → all 7 block types show distinct colors, no garbled text, borders/panel characters render correctly

### Implementation for US2 + US3

- [ ] T015 [US2] [US3] Update `source/Renderer.cpp` render() — add color to CHAR_INFO buffer:
  - Map each Color enum to a distinct FOREGROUND_* combination in CHAR_INFO.Attributes
  - Board cells: empty = FOREGROUND_BLACK | BACKGROUND_BLACK; occupied = corresponding color foreground on black background
  - Current piece: same color as its type, rendered on top of board cells
  - Board border: FOREGROUND_WHITE | BACKGROUND_BLACK
  - Side panel text: FOREGROUND_WHITE | BACKGROUND_BLACK
  - Score/lines/level labels: FOREGROUND_WHITE; values: FOREGROUND_BRIGHT_WHITE
- [ ] T016 [US2] [US3] Update `source/Renderer.cpp` init() — encoding handling:
  - Add SetConsoleOutputCP(CP_UTF8) 
  - Add SetConsoleCP(CP_UTF8)
  - Add error handling: if CP_UTF8 fails, fall back to GetConsoleOutputCP() and log warning
- [ ] T017 [US3] Verify display characters are ASCII-only in Renderer:
  - Block characters: '[' + ']' or '#' repeated
  - Border: '+', '-', '|'
  - All text labels: ASCII English only (Score, Lines, Level, Tetris, GAME OVER, Controls, etc.)
  - Zero emoji, zero Unicode block chars, zero special box-drawing chars
- [ ] T018 [US2] [US3] Verify all 7 piece colors are distinct and visible:
  - I=Cyan, O=Yellow, T=Magenta, S=Green, Z=Red, J=Blue, L=White/BrightWhite
  - Fixed blocks retain color after locking
  - Current falling piece color matches its type

**Checkpoint**: Game runs in both PowerShell and Windows Terminal with colors, no garbled text. All 7 piece types visually distinguishable.

---

## Phase 5: User Story 4 — Complete Piece System (Priority: P2)

**Goal**: All 7 tetrominoes spawn correctly (random/bag), rotation works properly, collision detection is robust

**Independent Test**: Play through many pieces → verify all 7 types appear; rotate each against walls → verify no wall penetration; verify pieces stack correctly without overlap

### Implementation for US4

- [ ] T019 [US4] Implement piece randomization in `source/Game.cpp` spawnPiece():
  - Simple approach: use std::mt19937 with std::uniform_int_distribution(0, 6) for random type
  - Create basic bag system (optional for v1): shuffle array of 7 types, refill when empty
- [ ] T020 [US4] Implement rotation collision in `source/Game.cpp` handleInput() rotate case:
  - Try rotate → if !board.canPlace(rotatedPiece) → revert rotation
  - Write a helper canRotate() or inline the try-revert pattern
- [ ] T021 [US4] Verify all edge cases in `source/Piece.cpp` + `source/Game.cpp`:
  - I-piece horizontal at row 0 (width 4, must not exceed bounds)
  - O-piece rotation (all 4 states identical, should still "rotate" without error)
  - S/Z-piece 2-state rotation cycle (rotate 0↔1, 2=0, 3=1)
  - Piece at left edge attempting left move → blocked
  - Piece at right edge attempting right move → blocked
  - Piece at bottom attempting moveDown → triggers lock
- [ ] T022 [US4] Implement hard drop scoring in `source/Game.cpp`:
  - In HardDrop handler: while(board.canPlace(piece.movedDown())) { piece.moveDown(); score += 2; }
  - Then lockPiece() immediately

**Checkpoint**: All 7 piece types appear and rotate correctly, collision detection is robust, hard drop adds score per cell dropped

---

## Phase 6: User Story 5 — Difficulty Scaling (Priority: P2)

**Goal**: Game speed increases as player clears more lines, providing progressive challenge

**Independent Test**: Clear lines continuously → verify level number increases → verify fall speed decreases measurably (from 800ms toward 100ms)

### Implementation for US5

- [ ] T023 [US5] Implement level calculation in `source/Game.cpp`:
  - level = lines / 10 + 1 (level 1 at 0-9 lines, level 2 at 10-19, etc.)
  - fallInterval = std::chrono::milliseconds(max(100, 800 - (level - 1) * 50))
- [ ] T024 [US5] Update `source/Renderer.cpp` render() — display level:
  - Add "Level: N" to side panel (already in layout from Phase 3, verify it updates)
- [ ] T025 [US5] Add game-over restart handling in `source/Game.cpp`:
  - In renderGameOver → after GAME OVER screen, poll for R key → reset all state (new Board, score=0, lines=0, level=1) → spawn first piece → resume loop
  - Q/Esc exits

**Checkpoint**: Level increases with line clears, speed increases smoothly, player can restart after game over

---

## Phase 7: User Story 6 — Code Modularity (Priority: P2)

**Goal**: Clean module separation, main.cpp is minimal, each module has clear single responsibility

**Independent Test**: Review code structure → verify 5 modules (Game/Board/Piece/Renderer/Input) each in own .h/.cpp; main.cpp is <30 lines; no cross-module implementation leakage

### Verification for US6

- [ ] T026 [US6] Review `source/main.cpp` — ensure ≤30 lines, only: includes, console setup, Game instantiation, game.run(), return
- [ ] T027 [US6] Review module boundaries:
  - Board knows nothing about rendering or input
  - Piece knows nothing about board or game
  - Renderer depends only on Board and Piece public interfaces
  - Input depends on nothing else
  - Game orchestrates all modules
- [ ] T028 [US6] Add `source/CMakeLists.txt` or verify existing one: all 6 .cpp files listed, C++17 standard, Windows subsystem optional

**Checkpoint**: Codebase is cleanly modular, ready for future spec-driven feature additions

---

## Phase 8: Polish & Final Verification

**Purpose**: End-to-end acceptance testing against spec.md criteria

- [ ] T029 Run acceptance tests from spec.md:
  - All US1 scenarios: block spawn/fall/move/rotate/lock/clear/end
  - All US2 scenarios: colors correct for all 7 types, fixed vs falling distinguishable
  - All US3 scenarios: no garbled text in GBK PowerShell + UTF-8 Terminal
  - All US4 scenarios: all 7 pieces spawn, rotation + collision correct
  - All US5 scenarios: level/speed scale correctly, restart works
- [ ] T030 Verify edge cases from spec.md:
  - I-piece horizontal at top row, boundary rotation
  - New piece spawn collision → game over trigger
  - Rapid key presses → no input loss
  - Hard drop → immediate lock + new piece spawn same frame
  - Multi-line clear (2, 3, 4 lines) → correct scoring
  - Score: 1=100, 2=300, 3=500, 4=800
- [ ] T031 Verify quickstart.md instructions: compile with g++ and MSVC, run in PowerShell and Windows Terminal, confirm zero garbled output
- [ ] T032 [P] Add `README.md` at repository root with project overview, build instructions, controls reference

---

## Dependencies & Execution Order

### Phase Dependencies

```
Phase 1: Setup ─────────────────────────────────────────────────────────────────────────────
  │
  ▼
Phase 2: Foundational (Board + Piece) ──── BLOCKS all gameplay ─────────────────────────────
  │
  ├──▶ Phase 3: US1 Core Gameplay (P1) 🎯 MVP ──── first playable build ───────────────────
  │      │
  │      ├──▶ Phase 4: US2+US3 Color & Encoding (P1) ── can overlap with US4 ───────────────
  │      │
  │      └──▶ Phase 5: US4 Complete Pieces (P2) ───── can overlap with US2/US3 ─────────────
  │             │
  │             └──▶ Phase 6: US5 Difficulty (P2) ───── depends on Game + Renderer ─────────
  │                    │
  │                    └──▶ Phase 7: US6 Modularity (P2 review) ──────────────────────────────
  │                           │
  │                           └──▶ Phase 8: Polish ──────────────────────────────────────────
```

### Within Each Phase

| Phase | Can Parallel |
|-------|-------------|
| Phase 1 | T002, T003 can run in parallel (T001 should be first) |
| Phase 2 | T004, T005 can run in parallel; T006 and T007 are independent of each other |
| Phase 3 | T008, T009, T010 can run in parallel; T012 before T013 |
| Phase 4 | All sequential (single file Renderer.cpp updates) |
| Phase 5 | All sequential (single file Game.cpp + Piece.cpp) |
| Phase 6 | All sequential |
| Phase 7 | All sequential (review) |
| Phase 8 | T032 independent |

### Critical Path (MVP)

```
T001 → T004 → T007 → T012 → T013 → T014 → 🎯 MVP playable
         ↘ T005 → T006 ↗       ↗
                          T008 ↗
                          T009 ↗
                          T010 ↗
                          T011 ↗
```

**MVP checkpoint at T014**: Monochrome but fully playable Tetris (~8 tasks)

---

## Parallel Opportunities

```bash
# Phase 1 - Run together:
Task: T002 "Create source/Color.h"
Task: T003 "Create source/Input.h"

# Phase 2 - Run together:
Task: T004 "Create source/Board.h"
Task: T005 "Create source/Piece.h"
# Then:
Task: T006 "Implement source/Piece.cpp"
Task: T007 "Implement source/Board.cpp"

# Phase 3 - Run together:
Task: T008 "Implement source/Input.cpp"
Task: T009 "Implement source/Renderer.h"
Task: T010 "Implement source/Colors.h"
```

## Implementation Strategy

### MVP First (Recommended)

1. Phase 1: Setup → compile empty project
2. Phase 2: Board + Piece → data layer ready
3. Phase 3: Game loop → **🎯 First playable Tetris (monochrome)**
4. **STOP and VALIDATE**: Full gameplay works, all controls, line clear, game over
5. Phase 4: Add colors → visual polish
6. Phase 5-6: Add full piece system + difficulty → complete v1
7. Phase 8: Final verification

### Tasks Summary

| Phase | Tasks | Story | Deliverable |
|-------|-------|-------|-------------|
| 1: Setup | T001-T003 | — | Build system + type definitions |
| 2: Foundational | T004-T007 | US4 | Board + Piece data layer |
| 3: US1 Core | T008-T014 | US1 | 🎯 Playable monochrome Tetris |
| 4: US2+US3 Color | T015-T018 | US2, US3 | Color display + encoding compat |
| 5: US4 Pieces | T019-T022 | US4 | Full 7-piece + rotation system |
| 6: US5 Difficulty | T023-T025 | US5 | Level scaling + restart |
| 7: US6 Modularity | T026-T028 | US6 | Code structure review |
| 8: Polish | T029-T032 | — | Final acceptance testing |

**Total: 32 tasks across 8 phases**
