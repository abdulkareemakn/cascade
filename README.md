# Cascade

A personal task manager CLI application built with C++23, demonstrating advanced data structures and algorithms including graphs, heaps, and sorting.

## Features

- **Task Management**: Create, update, delete, and list tasks with priorities and due dates
- **Dependency Tracking**: Define dependencies between tasks with automatic cycle detection
- **Smart Prioritization**: Get the next task to work on based on priority and deadline (MinHeap)
- **Execution Planning**: Generate task execution order respecting all dependencies (Topological Sort)
- **Critical Path Analysis**: Identify the longest dependency chain in your project

## Installation

### Prerequisites

- C++23 compatible compiler (clang++ recommended)
- CMake 4.0+
- SQLite3

### Build

```bash
cmake -B build && cmake --build build
```

The executable will be created at `./build/cascade`.

## Usage

### Task Management

```bash
# Create a task
cascade task add "Write documentation"
cascade task add "Review code" -p 1 --due tomorrow
cascade task add "Deploy app" --priority 2 --due 2025-12-20

# List tasks
cascade task list                    # Show incomplete tasks
cascade task list --all              # Include completed tasks
cascade task list --sort priority    # Sort by priority
cascade task list --sort date        # Sort by due date

# View a specific task
cascade task show 1

# Get next priority task (uses MinHeap)
cascade task next

# Update tasks
cascade task update 1 --priority 1
cascade task update 1 --status in_progress
cascade task update 1 --due next-week
cascade task update 1 --title "New title"

# Quick status changes
cascade task start 1                 # Mark as in progress
cascade task done 1                  # Mark as complete

# Delete a task
cascade task delete 1
```

### Dependency Management

```bash
# Add a dependency (task 2 depends on task 1)
cascade deps add 2 1

# Remove a dependency
cascade deps remove 2 1

# View dependencies for a task
cascade deps show 2

# Generate execution plan (Topological Sort)
cascade deps plan

# Analyze critical path
cascade deps critical
```

### Options Reference

| Option | Values |
|--------|--------|
| `--priority, -p` | 1 (highest) to 4 (lowest) |
| `--status` | todo, in_progress, complete, wont_do |
| `--due` | YYYY-MM-DD, today, tomorrow, next-week, next-month |
| `--sort` | priority, date, created |

## Data Structures and Algorithms

This project demonstrates the following DSA concepts:

| Component | Data Structure | Algorithm |
|-----------|---------------|-----------|
| Task prioritization | Min-Heap | Heap operations (insert, extractMin) |
| Task dependencies | Adjacency List (Graph) | DFS cycle detection, Kahn's topological sort |
| Critical path | DAG | Dynamic programming on topological order |
| Task sorting | Vector | Mergesort with custom comparators |

## Project Structure

```
cascade/
├── include/          # Header files
├── src/              # Implementation files
├── lib/              # Third-party dependencies
├── db/               # SQLite database (created at runtime)
└── docs/             # Documentation
```

## Dependencies

- [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp) - SQLite wrapper (MIT)
- [CLI11](https://github.com/CLIUtils/CLI11) - Command line parser (BSD-3-Clause)
- [tabulate](https://github.com/p-ranav/tabulate) - Table formatting (MIT)
- [fmt](https://github.com/fmtlib/fmt) - Formatting library (MIT)

## License

MIT License. See [LICENSE](LICENSE) for details.
