#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "models.h"

namespace core {

struct CycleResult {
    bool hasCycle;
    std::vector<int>
        cyclePath;  
};

struct CriticalPathResult {
    std::vector<int> path;  
    int length;             
};

class TaskGraph {
   public:
    TaskGraph() = default;

    void addTask(int taskId);
    void addTask(const Task& task);
    void removeTask(int taskId);

    bool addDependency(int taskId, int dependsOnId);
    void removeDependency(int taskId, int dependsOnId);

    bool hasTask(int taskId) const;
    bool hasDependency(int taskId, int dependsOnId) const;
    std::vector<int> getDependencies(
        int taskId) const;  
    std::vector<int> getDependents(
        int taskId) const;  
    std::vector<int> getAllTaskIds() const;
    int getTaskCount() const;
    int getDependencyCount() const;

    
    CycleResult detectCycle() const;

    bool wouldCreateCycle(int taskId, int dependsOnId) const;

    std::vector<int> topologicalSort() const;

    CriticalPathResult criticalPath() const;

    void loadFromDependencies(const std::vector<TaskDependency>& dependencies);

    void clear();

   private:
    std::unordered_map<int, std::unordered_set<int>> adjacencyList_;

    std::unordered_map<int, std::unordered_set<int>> reverseAdjacencyList_;

    std::unordered_map<int, Task> tasks_;

    enum class Color { WHITE, GRAY, BLACK };

    bool dfsDetectCycle(int taskId, std::unordered_map<int, Color>& colors,
                        std::vector<int>& path,
                        std::vector<int>& cyclePath) const;

    int dfsLongestPath(int taskId, std::unordered_map<int, int>& memo,
                       std::unordered_map<int, int>& nextOnPath) const;
};

}  // namespace core