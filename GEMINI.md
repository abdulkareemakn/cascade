⚙️ Gemini CLI Development Guide for Cascade Project

This document outlines the operational guidelines for using the Gemini CLI as the primary development and consultation tool for the Cascade Multi-User Task Management System (based on the provided Requirements Document).

1. Project Overview

Project Name: Cascade - Multi-User Task Management System

Primary Language: C++23

Build System: CMake

Primary Focus: Implementation and demonstration of advanced Data Structures and Algorithms (Graphs, Heaps, Sorting, etc.) as detailed in the project requirements (Section 4.1, 4.2).

Persistence: SQLite (via sqlite_orm library).

2. Gemini CLI Protocol

To maximize efficiency and minimize unnecessary context in the CLI, please adhere to the following interaction rules:

A. Code Generation Policy (Mandatory)

Unless explicitly asked for, NO code should be generated.

My responses should focus on theoretical explanations, architectural decisions, C++ best practices, algorithm analysis, and database schema design.

Example (Good): "Explain the time complexity implications of using an adjacency list vs. an adjacency matrix for task dependencies (DSA-03)."

Example (Bad): "Write the C++ code for the topological sort algorithm."

B. Context and Focus

Requirement Reference: When discussing a feature, please reference the Functional Requirement (FR) or Data Structure/Algorithm (DSA) ID from the Requirements Document whenever possible (e.g., "Discuss the implementation strategy for FR-TM-07" or "How should we implement DSA-02?").

Explicit File Targeting: If a response involves modifying an existing file (CMakeLists.txt, database.h, etc.), please state the file name clearly.

Conventional Commits: All conceptual discussions will assume adherence to the Conventional Commits specification for git messages (e.g., feat:, fix:, build:).

3. Core Development Areas for CLI Assistance

We will focus our theoretical discussions on the most complex requirements of the project.

3.1 Data Persistence & Schema (database.cpp)

Goal: Design the SQLite schema using sqlite_orm to satisfy requirements for Users, Tasks, Teams, and especially Task Dependencies (FR-TD-01).

Focus Areas:

Table relationships (one-to-many, many-to-many).

Designing the graph tables (nodes/edges) for dependencies (FR-TD-03).

Password hashing and storage (FR-UM-03, NFR-S-01).

3.2 Task Dependencies (Graph Theory)

The core complexity lies here (Section 2.6, DSA-03, DSA-09-12).

Data Structure: Directed Graph using an Adjacency List (DSA-03).

Critical Algorithms:

Cycle Detection (FR-TD-05): Required to prevent impossible dependency chains. We will discuss the DFS-based approach using three colors (White, Gray, Black) for state tracking.

Topological Sort (FR-TD-08, DSA-11): Required to calculate the optimal completion order. We will analyze the implementation of Kahn's Algorithm (using in-degrees) versus the DFS-based approach.     * Critical Path Analysis (FR-TD-15): Identifying the longest dependency chain to determine the minimum time required for project completion.

3.3 Task Organization & Prioritization

Priority Queue (DSA-02): We need to discuss the custom implementation of a Min-Heap/Priority Queue to retrieve the next most urgent task, prioritizing based on both priority level and deadline (FR-PD-03).

Sorting Algorithms (DSA-07): Analyze the pros and cons of implementing Quicksort vs. Mergesort for sorting large lists of tasks by date or priority (FR-TO-01).

Your next query should be a specific question related to one of the development areas above.
