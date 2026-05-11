# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and run

- Configure a fresh build directory with Ninja:
  - `cmake -S . -B build -G Ninja`
- Build the project:
  - `cmake --build build`
- List configured tests:
  - `ctest --test-dir build -N`
- Run tests:
  - There are currently no CTest tests configured (`ctest --test-dir build -N` reports `Total Tests: 0`).
- Run the application:
  - Run it from the directory that contains the runtime data files it expects.
  - The executable built by CMake is `build/fuck_zhku_homework.exe`.
  - The program opens `records.txt`, `users.txt`, and `species.txt` via relative paths, so the current working directory matters.

## Runtime data files

- `proj/io.c` hard-codes these filenames:
  - `records.txt` for persisted water-quality records
  - `species.txt` for species restriction ranges
- `proj/user_management.c` hard-codes `users.txt` for account storage.
- The checked-in repository root currently has `records.txt` and `users.txt`, but the species file in root is named `species.txt.txt`, while the generated `build/` directory contains `species.txt`.
- Practical implication: if you run from the repository root, the program may fail to load restriction data unless `species.txt` exists there. Running from `build/` matches the current generated runtime layout.

## Architecture overview

### Entry point and startup flow

- `main.c` is minimal: it calls `readRestrictionData()` first, then `initConsole()`.
- Startup order matters because the rest of the program assumes the global per-species restriction tables have already been populated.

### Core state and control flow

- `proj/console.c` is the orchestration layer for the whole application.
- It owns two important globals declared in `proj/console.h`:
  - `globalRecordList`: the in-memory `ArrayList` of `struct WaterQuality*`
  - `mode`: the currently selected species mode
- `initConsole()` loads persisted records, initializes terminal behavior, starts the login flow, shows the species-selection screen, then enters the main user menu.
- Most user-visible features are implemented as menu handlers in `proj/console.c`: viewing, editing, deleting, adding records, monitoring, statistics, password change, and admin-only user management.

### Data model and collection layer

- `proj/data.h` defines the main domain model: `struct WaterQuality`.
- `lib/array_list.[ch]` is the project’s shared container abstraction. Most modules exchange records and users through `struct ArrayList` rather than through typed collections.
- The list stores raw pointers, and many code paths assume ownership manually, so memory-management bugs are most likely when changing CRUD flows.

### Persistence and data generation

- `proj/io.c` is responsible for:
  - loading/saving water-quality records
  - loading species restriction ranges from file
  - generating simulated monitoring data
- `watchInit()` in `proj/console.c` clears `globalRecordList` and replaces it with the generated dataset from `generateRandomWaterQualityData()`.
- Record persistence is only written back on the “exit and save” path in `userLoopInit()`.

### Validation and species restrictions

- `proj/data_restriction.[ch]` is the canonical validation/classification layer.
- It stores three range sets per species as globals:
  - normal range
  - alert range
  - valid-input range
- Functions such as `checkPenaeusVannameiData()` classify a record as `NORMAL`, `NORMAL_ALERT`, `SERIOUS_ALERT`, or `INVALID_DATA`.
- Editing and manual record entry rely on this layer to reject invalid values.

### Warning and statistics features

- `proj/statistics.c` computes aggregates from `globalRecordList` using the currently selected species restriction globals.
- `proj/water_warning.c` renders the “intelligent warning” screen shown after species selection.
- Important non-obvious detail: `proj/water_warning.c` uses its own hard-coded `SpeciesConfig` thresholds instead of reusing the file-driven restriction data loaded by `proj/io.c` / `proj/data_restriction.c`. If warning behavior and validation behavior diverge, check both systems.

### User system

- `proj/user_management.c` maintains an in-memory `user_list` mirrored to `users.txt`.
- If `users.txt` does not exist, the module seeds a default admin account: `admin / 123456`.
- Admin-only behavior is enforced in the UI layer (`manageUsers()`) and in `deleteUserByUsername()`.
- Passwords are stored in plaintext; this is current behavior, not an abstraction boundary.

## Platform assumptions

- This is a Windows-specific console application.
- `proj/console.c` and `proj/user_management.c` depend on `windows.h`, `conio.h`, `_getch()`, `Sleep()`, console code page changes, and Win32 cursor control.
- The generated CMake cache shows the project is currently built with MinGW GCC and Ninja, with C23/C++23 enabled in `CMakeLists.txt`.

## Working notes for future edits

- Prefer tracing feature behavior from `proj/console.c` outward, because it is the hub for menu actions and state transitions.
- When changing validation or alert behavior, check all three places that can affect outcomes:
  - `proj/io.c` for restriction loading and generated sample data
  - `proj/data_restriction.c` for canonical validation/classification
  - `proj/water_warning.c` for the separate warning dashboard thresholds
- There is no automated test suite yet, so the main verification path is rebuilding with CMake and then exercising the console flow manually.