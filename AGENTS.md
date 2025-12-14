# AGENTS.md - Cascade Project

## Project Overview
Cascade is a **Personal Task Manager** (single-user) focused on demonstrating advanced DSA (Graphs, Heaps, Sorting). Persistence via SQLite (SQLiteCpp).

**Version**: 1.0.0

## Current Status
**Phase 3 Complete - CLI Application Ready** - Full-featured CLI with all DSA algorithms exposed through user-friendly commands.

### What's Done

✅ **Database Layer (Complete - Single User)**
- Schema: `users`, `tasks`, `task_dependencies` tables
- User CRUD: create, get (by username/email/id), update, delete, authenticate
- Task CRUD: create, get, update (status/priority/dueDate/title), delete
- Task Dependencies: add/remove dependencies, get dependencies/dependents
- `db::getIncompleteTasksByUser()` - filters tasks by status (TODO, IN_PROGRESS only)
- All operations use prepared statements with proper foreign key cascades
- Singleton connection pattern via `db::getConnection()`
- `security::hashPassword()` and `verifyPassword()` using Argon2

✅ **DSA - Priority Queue (MinHeap) - DSA-02, FR-PD-03**
- Implementation: `include/PriorityQueue.h`, `src/PriorityQueue.cpp`
- Stores `std::vector<Task>`, compares by `(priority, dueDate)` - lower values = higher priority
- Operations: `insert()`, `extractMin()`, `peek()`, `isEmpty()`, `getSize()`
- Exposed via `cascade task next` command

✅ **DSA - Sorting Algorithms (DSA-07, FR-TO-01)**
- Implementation: `include/sorting.h`, `src/sorting.cpp`
- **Mergesort**: `core::mergeSort()` with custom comparators, stable sort
- Comparators: `byPriority`, `byDueDate`, `byId`
- Exposed via `cascade task list --sort <priority|date|created>`

✅ **DSA - Graph Algorithms (DSA-03)**
- Implementation: `include/TaskGraph.h`, `src/TaskGraph.cpp`
- **Adjacency List**: `std::unordered_map<int, std::vector<int>>` for task dependencies
- **Cycle Detection** (FR-TD-05): DFS with 3-color marking (WHITE, GRAY, BLACK)
- **Topological Sort** (FR-TD-08, DSA-11): Kahn's algorithm (BFS-based)
- **Critical Path Analysis** (FR-TD-15): Longest dependency chain using topological sort + DP
- Exposed via `cascade deps plan` and `cascade deps critical`

✅ **Integration Layer (repository.cpp)**
- Implementation: `include/repository.h`, `src/repository.cpp`
- Bridges Database <-> DSA layers for application-level operations
- Display functions: `showTask()`, `showAllTasks()`, `listTasks()`, `getNextPriorityTask()`
- Update functions: `updateTaskPriority()`, `updateTaskStatus()`, `updateTaskDueDate()`, `updateTaskTitle()`, `deleteTask()`
- Dependency functions: `addDependency()`, `removeDependency()`, `showDependencies()`, `showExecutionPlan()`, `showCriticalPath()`

✅ **Utility Functions (util.cpp)**
- `parseDate()` - Supports YYYY-MM-DD and relative dates (today, tomorrow, next-week, next-month)
- `formatDate()` - Converts timestamps to YYYY-MM-DD for display
- `statusToString()` - Converts status int to readable string (TODO, IN_PROGRESS, COMPLETE, WONT_DO)
- `taskStatusToInt()` - Parses status string to int
- `styleTable()` - Applies clean minimal formatting to tabulate tables

✅ **CLI Application (Complete)**
- Implementation: `src/main.cpp`, `include/commands.h`
- Git-style subcommands using CLI11
- Clean table output using tabulate (minimal borders, bold headers)
- Version flag: `-v, --version`

### CLI Commands

```bash
# User
cascade whoami                              # Show current user

# Task Management
cascade task add "Title" [-p 1-4] [--due DATE]   # Create task
cascade task list [--all] [--status N] [--sort S] # List tasks
cascade task show <id>                      # Show task details
cascade task next                           # Get highest priority task (MinHeap)
cascade task update <id> [--priority] [--status] [--due] [--title]
cascade task done <id>                      # Mark task complete
cascade task start <id>                     # Mark task in progress
cascade task delete <id>                    # Delete task

# Dependencies (Graph Algorithms)
cascade deps add <id> <depends-on-id>       # Add dependency (with cycle detection)
cascade deps remove <id> <depends-on-id>    # Remove dependency
cascade deps show <id>                      # Show task dependencies
cascade deps plan                           # Execution order (topological sort)
cascade deps critical                       # Critical path analysis
```

**Options**:
- `--due`: YYYY-MM-DD or today, tomorrow, next-week, next-month
- `--status`: todo, in_progress, complete, wont_do
- `--sort`: priority, date, created
- `--all`: Include completed tasks in list

### Important Notes
- **SQLite connections**: Using singleton pattern. Use `auto& db = db::getConnection();` (reference, not copy)
- **Foreign keys**: `PRAGMA foreign_keys = ON;` enabled on every connection
- **Prepared statements**: Binding is 1-indexed
- **Timestamps**: Stored as INTEGER (Unix epoch)
- **TaskStatus**: Only TODO and IN_PROGRESS returned by `getIncompleteTasksByUser()`
- See `docs/SQL.md` for SQLite pitfalls and best practices

## Build Commands
```bash
cmake -B build && cmake --build build      # Full build
cmake --build build                         # Incremental build
./build/cascade                             # Run CLI
./build/cascade --version                   # Show version
rm -f db/cascade.db                         # Delete database for fresh start
```

## Code Style (Google C++ with modifications)
- **Formatter**: clang-format (BasedOnStyle: Google, IndentWidth: 4, UseTab: Never)
- **Standard**: C++23 with clang++/libc++
- **Includes**: Standard library first, then local headers (`"database.h"`)
- **Namespaces**: `db::`, `security::`, `repo::`, `core::`
- **Types**: Use `std::string`, `std::optional`, `std::time_t`; structs for models
- **Naming**: snake_case for variables/functions, PascalCase for types/structs
- **Error handling**: Exceptions with `std::runtime_error`, return `bool` or `std::optional`
- **Headers**: Use `#pragma once`, models in `include/models.h`
- **Git**: Conventional Commits (`feat:`, `fix:`, `build:`)

## Project Structure
```
include/         # Headers
src/             # Implementation
lib/             # Third-party libraries
db/              # SQLite database (created at runtime)
docs/            # Documentation
```

**Dependencies**:
- SQLite3 (system, via SQLiteCpp wrapper in `lib/sqlitecpp/`)
- Argon2 (vendored in `lib/argon2/`)
- CLI11 (vendored in `lib/CLI11/`, header-only)
- tabulate (vendored in `lib/tabulate/`, header-only)
- fmt (vendored in `lib/fmt/`)

## Core DSA Requirements (All Complete)
- **Graph (Adjacency List)**: Task dependencies (DSA-03)
  - Cycle detection via DFS (FR-TD-05)
  - Topological sort (FR-TD-08, DSA-11)
  - Critical path analysis (FR-TD-15)
- **Priority Queue (Min-Heap)**: Task prioritization by priority+deadline (DSA-02, FR-PD-03)
- **Sorting**: Mergesort for task lists by date/priority (DSA-07, FR-TO-01)

## Agent Policy
**DO NOT generate or modify code unless explicitly requested.** Focus on explanations, architecture, algorithm analysis, and schema design. Reference FR/DSA IDs when discussing features.

When code generation is requested:
- Follow all code style conventions above
- Use namespace organization (`db::`, `security::`, `repo::`, `core::`)
- Write comprehensive tests after implementation
- Update this file with progress

## Future Improvements

The following items are known technical debt or enhancements to consider if revisiting this project:

### Architecture
- **Separate concerns in `repo::` layer**: Currently, repository functions print messages directly to stdout, mixing business logic with presentation. Refactor to return result types (e.g., `std::expected` or custom Result structs) and let the CLI layer handle user messaging.
- **Error handling consistency**: `db::` functions both print exceptions and return bool/optional. Consider a unified error handling strategy.

### Testing
- **Unit tests needed**: No test suite exists. Priority areas:
  - `core::Queue` (PriorityQueue) - heap property verification
  - `core::TaskGraph` - cycle detection, topological sort, critical path
  - `core::mergeSort` - sorting correctness with various comparators
  - `db::` functions - database operations (use in-memory SQLite for tests)

### Input Validation
- **`parseDate()`**: Silently returns current time on invalid input. Consider returning `std::optional` or throwing.
- **Priority validation**: Currently only validated in `repo::updateTaskPriority()`, not in `db::createTask()`.

### Database
- **No transactions**: Multi-step operations (e.g., adding dependency with validation) don't use transactions.
- **Schema constraints**: `task_dependencies` table lacks foreign key constraints to `tasks` table.
