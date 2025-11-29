# AGENTS.md - Cascade Project

## Project Overview
Cascade is a Multi-User Task Management System focused on demonstrating advanced DSA (Graphs, Heaps, Sorting). Persistence via SQLite (SQLiteCpp).

## Current Status
**Database layer in progress** - Implementing CRUD operations with SQLiteCpp (switched from sqlite_orm for less abstraction).

### What's Done
- Database schema: `users`, `tasks`, `teams` tables with foreign keys
- `initStorage()` creates tables with `IF NOT EXISTS`
- Singleton connection pattern via `db::getConnection()` (TODO: replace with connection pool for multi-user)
- `security::hashPassword()` and `verifyPassword()` using Argon2

### What's Next
- Implement `db::createUser()` - INSERT with prepared statement, hash password
- Implement remaining User CRUD: `getUser()`, `getUserById()`, `deleteUser()`
- Implement Task CRUD operations
- Implement Team CRUD operations
- Then move to DSA layer (MinHeap, Graph for dependencies)

### Important Notes
- **SQLite connections**: Using singleton pattern (see `database.cpp:32`). Use `auto& db = db::getConnection();` (reference, not copy)
- **Foreign keys**: Must run `PRAGMA foreign_keys = ON;` on every connection
- **Prepared statements**: Binding is 1-indexed, use `statement.bind(1, value)`
- **Timestamps**: Store as INTEGER, use `unixepoch()` in SQL
- See `docs/SQL.md` for SQLite pitfalls and best practices

## Build Commands
```bash
cmake -B build && cmake --build build      # Full build
cmake --build build                         # Incremental build
./build/cascade                             # Run executable
```

## Code Style (Google C++ with modifications)
- **Formatter**: clang-format (BasedOnStyle: Google, IndentWidth: 4, UseTab: Never)
- **Standard**: C++23 with clang++/libc++
- **Includes**: Standard library first, then local headers (`"database.h"`)
- **Namespaces**: Use namespaces for modules (e.g., `namespace security`, `namespace db`)
- **Types**: Use `std::string`, `std::optional`, `std::time_t`; structs for models
- **Naming**: snake_case for variables/functions, PascalCase for types/structs
- **Error handling**: Exceptions with `std::runtime_error`, return `bool` or `std::optional`
- **Headers**: Use `#pragma once`, models in `include/models.h`
- **Git**: Conventional Commits (`feat:`, `fix:`, `build:`)

## Project Structure
`include/` headers, `src/` implementation, `lib/` third-party. Deps: SQLite3 (via SQLiteCpp), OpenSSL, Argon2.

## Core DSA Requirements
- **Graph (Adjacency List)**: Task dependencies (DSA-03), cycle detection via DFS (FR-TD-05), topological sort (FR-TD-08, DSA-11), critical path analysis (FR-TD-15)
- **Priority Queue (Min-Heap)**: Task prioritization by priority+deadline (DSA-02, FR-PD-03). Store `std::vector<Task>`, compare by `(priority, dueDate)`
- **Sorting**: Quicksort/Mergesort for task lists by date/priority (DSA-07, FR-TO-01)

## Agent Policy
**DO NOT generate or modify code unless explicitly requested.** Focus on explanations, architecture, algorithm analysis, and schema design. Reference FR/DSA IDs when discussing features.
