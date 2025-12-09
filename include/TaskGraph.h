#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "models.h"

namespace core {

// Result structure for cycle detection - contains the cycle path if found
struct CycleResult {
    bool hasCycle;
    std::vector<int>
        cyclePath;  // Task IDs forming the cycle (empty if no cycle)
};

// Result structure for critical path analysis
struct CriticalPathResult {
    std::vector<int> path;  // Task IDs on the critical path
    int length;             // Length of the critical path (number of tasks)
};

class TaskGraph {
   public:
    TaskGraph() = default;

    // Graph construction
    void addTask(int taskId);
    void addTask(const Task& task);
    void removeTask(int taskId);

    // Edge management (dependency: taskId depends on dependsOnId)
    // Returns false if adding this edge would create a cycle
    bool addDependency(int taskId, int dependsOnId);
    void removeDependency(int taskId, int dependsOnId);

    // Query methods
    bool hasTask(int taskId) const;
    bool hasDependency(int taskId, int dependsOnId) const;
    std::vector<int> getDependencies(
        int taskId) const;  // Tasks this task depends on
    std::vector<int> getDependents(
        int taskId) const;  // Tasks that depend on this task
    std::vector<int> getAllTaskIds() const;
    int getTaskCount() const;
    int getDependencyCount() const;

    // DSA-03: Core Graph Algorithms

    // FR-TD-05: Cycle Detection using DFS with 3-color marking
    // WHITE (0) = unvisited, GRAY (1) = in current path, BLACK (2) = fully
    // processed
    CycleResult detectCycle() const;

    // Check if adding a dependency would create a cycle (without actually
    // adding it)
    bool wouldCreateCycle(int taskId, int dependsOnId) const;

    // FR-TD-08, DSA-11: Topological Sort using Kahn's algorithm
    // Returns empty vector if graph has a cycle
    std::vector<int> topologicalSort() const;

    // FR-TD-15: Critical Path Analysis
    // Returns the longest path through the dependency graph
    CriticalPathResult criticalPath() const;

    // Utility: Build graph from database dependencies
    void loadFromDependencies(const std::vector<TaskDependency>& dependencies);

    // Clear the graph
    void clear();

   private:
    // Adjacency list: taskId -> set of task IDs it depends on
    std::unordered_map<int, std::unordered_set<int>> adjacencyList_;

    // Reverse adjacency list: taskId -> set of task IDs that depend on it
    std::unordered_map<int, std::unordered_set<int>> reverseAdjacencyList_;

    // Store task metadata (optional, for convenience)
    std::unordered_map<int, Task> tasks_;

    // Helper for cycle detection - DFS with 3 colors
    enum class Color { WHITE, GRAY, BLACK };

    bool dfsDetectCycle(int taskId, std::unordered_map<int, Color>& colors,
                        std::vector<int>& path,
                        std::vector<int>& cyclePath) const;

    // Helper for critical path - DFS to find longest path from a node
    int dfsLongestPath(int taskId, std::unordered_map<int, int>& memo,
                       std::unordered_map<int, int>& nextOnPath) const;
};

}  // namespace core
