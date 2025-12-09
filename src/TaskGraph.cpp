#include "TaskGraph.h"

#include <queue>

namespace core {

void TaskGraph::addTask(int taskId) {
    if (adjacencyList_.find(taskId) == adjacencyList_.end()) {
        adjacencyList_[taskId] = {};
        reverseAdjacencyList_[taskId] = {};
    }
}

void TaskGraph::addTask(const Task& task) {
    addTask(task.id);
    tasks_[task.id] = task;
}

void TaskGraph::removeTask(int taskId) {
    // Remove all dependencies involving this task
    for (auto& [id, deps] : adjacencyList_) {
        deps.erase(taskId);
    }
    for (auto& [id, deps] : reverseAdjacencyList_) {
        deps.erase(taskId);
    }

    adjacencyList_.erase(taskId);
    reverseAdjacencyList_.erase(taskId);
    tasks_.erase(taskId);
}

// ============================================================================
// Edge Management
// ============================================================================

bool TaskGraph::addDependency(int taskId, int dependsOnId) {
    // Ensure both tasks exist
    addTask(taskId);
    addTask(dependsOnId);

    // Check if this would create a cycle
    if (wouldCreateCycle(taskId, dependsOnId)) {
        return false;
    }

    adjacencyList_[taskId].insert(dependsOnId);
    reverseAdjacencyList_[dependsOnId].insert(taskId);
    return true;
}

void TaskGraph::removeDependency(int taskId, int dependsOnId) {
    if (adjacencyList_.find(taskId) != adjacencyList_.end()) {
        adjacencyList_[taskId].erase(dependsOnId);
    }
    if (reverseAdjacencyList_.find(dependsOnId) !=
        reverseAdjacencyList_.end()) {
        reverseAdjacencyList_[dependsOnId].erase(taskId);
    }
}

// ============================================================================
// Query Methods
// ============================================================================

bool TaskGraph::hasTask(int taskId) const {
    return adjacencyList_.find(taskId) != adjacencyList_.end();
}

bool TaskGraph::hasDependency(int taskId, int dependsOnId) const {
    auto it = adjacencyList_.find(taskId);
    if (it == adjacencyList_.end()) {
        return false;
    }
    return it->second.find(dependsOnId) != it->second.end();
}

std::vector<int> TaskGraph::getDependencies(int taskId) const {
    auto it = adjacencyList_.find(taskId);
    if (it == adjacencyList_.end()) {
        return {};
    }
    return std::vector<int>(it->second.begin(), it->second.end());
}

std::vector<int> TaskGraph::getDependents(int taskId) const {
    auto it = reverseAdjacencyList_.find(taskId);
    if (it == reverseAdjacencyList_.end()) {
        return {};
    }
    return std::vector<int>(it->second.begin(), it->second.end());
}

std::vector<int> TaskGraph::getAllTaskIds() const {
    std::vector<int> ids;
    ids.reserve(adjacencyList_.size());
    for (const auto& [id, _] : adjacencyList_) {
        ids.push_back(id);
    }
    return ids;
}

int TaskGraph::getTaskCount() const {
    return static_cast<int>(adjacencyList_.size());
}

int TaskGraph::getDependencyCount() const {
    int count = 0;
    for (const auto& [_, deps] : adjacencyList_) {
        count += static_cast<int>(deps.size());
    }
    return count;
}

// ============================================================================
// FR-TD-05: Cycle Detection using DFS with 3-color marking
// ============================================================================

CycleResult TaskGraph::detectCycle() const {
    std::unordered_map<int, Color> colors;

    // Initialize all nodes as WHITE (unvisited)
    for (const auto& [taskId, _] : adjacencyList_) {
        colors[taskId] = Color::WHITE;
    }

    std::vector<int> path;
    std::vector<int> cyclePath;

    // Run DFS from each unvisited node
    for (const auto& [taskId, _] : adjacencyList_) {
        if (colors[taskId] == Color::WHITE) {
            if (dfsDetectCycle(taskId, colors, path, cyclePath)) {
                return {true, cyclePath};
            }
        }
    }

    return {false, {}};
}

bool TaskGraph::dfsDetectCycle(int taskId,
                               std::unordered_map<int, Color>& colors,
                               std::vector<int>& path,
                               std::vector<int>& cyclePath) const {
    colors[taskId] = Color::GRAY;  // Mark as being processed
    path.push_back(taskId);

    auto it = adjacencyList_.find(taskId);
    if (it != adjacencyList_.end()) {
        for (int neighbor : it->second) {
            if (colors[neighbor] == Color::GRAY) {
                // Found a back edge - cycle detected!
                // Extract the cycle from path
                cyclePath.clear();
                bool inCycle = false;
                for (int node : path) {
                    if (node == neighbor) {
                        inCycle = true;
                    }
                    if (inCycle) {
                        cyclePath.push_back(node);
                    }
                }
                cyclePath.push_back(neighbor);  // Complete the cycle
                return true;
            } else if (colors[neighbor] == Color::WHITE) {
                if (dfsDetectCycle(neighbor, colors, path, cyclePath)) {
                    return true;
                }
            }
            // If BLACK, already fully processed - skip
        }
    }

    colors[taskId] = Color::BLACK;  // Mark as fully processed
    path.pop_back();
    return false;
}

bool TaskGraph::wouldCreateCycle(int taskId, int dependsOnId) const {
    // Adding edge: taskId -> dependsOnId (taskId depends on dependsOnId)
    // This creates a cycle if there's already a path from taskId to dependsOnId
    // following existing dependency edges (i.e., dependsOnId already depends on
    // taskId directly or transitively)

    if (taskId == dependsOnId) {
        return true;  // Self-loop
    }

    // BFS from dependsOnId following its dependencies to see if we reach taskId
    // If dependsOnId (transitively) depends on taskId, adding
    // taskId->dependsOnId creates cycle
    std::unordered_set<int> visited;
    std::queue<int> queue;
    queue.push(dependsOnId);

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();

        if (current == taskId) {
            return true;  // Found: dependsOnId depends on taskId (transitively)
        }

        if (visited.find(current) != visited.end()) {
            continue;
        }
        visited.insert(current);

        // Follow forward edges (dependencies of current)
        auto it = adjacencyList_.find(current);
        if (it != adjacencyList_.end()) {
            for (int dependency : it->second) {
                if (visited.find(dependency) == visited.end()) {
                    queue.push(dependency);
                }
            }
        }
    }

    return false;
}

// ============================================================================
// FR-TD-08, DSA-11: Topological Sort using Kahn's Algorithm
// ============================================================================

std::vector<int> TaskGraph::topologicalSort() const {
    if (adjacencyList_.empty()) {
        return {};
    }

    // Calculate in-degrees (number of dependencies each task has)
    std::unordered_map<int, int> inDegree;
    for (const auto& [taskId, _] : adjacencyList_) {
        inDegree[taskId] = 0;
    }

    for (const auto& [taskId, deps] : adjacencyList_) {
        inDegree[taskId] = static_cast<int>(deps.size());
    }

    // Queue of tasks with no dependencies (in-degree 0)
    std::queue<int> ready;
    for (const auto& [taskId, degree] : inDegree) {
        if (degree == 0) {
            ready.push(taskId);
        }
    }

    std::vector<int> sorted;
    sorted.reserve(adjacencyList_.size());

    while (!ready.empty()) {
        int current = ready.front();
        ready.pop();
        sorted.push_back(current);

        // For each task that depends on current, reduce its in-degree
        auto it = reverseAdjacencyList_.find(current);
        if (it != reverseAdjacencyList_.end()) {
            for (int dependent : it->second) {
                inDegree[dependent]--;
                if (inDegree[dependent] == 0) {
                    ready.push(dependent);
                }
            }
        }
    }

    // If we couldn't process all nodes, there's a cycle
    if (sorted.size() != adjacencyList_.size()) {
        return {};  // Cycle detected
    }

    return sorted;
}

// ============================================================================
// FR-TD-15: Critical Path Analysis
// ============================================================================

CriticalPathResult TaskGraph::criticalPath() const {
    if (adjacencyList_.empty()) {
        return {{}, 0};
    }

    // Check for cycles first
    auto cycleResult = detectCycle();
    if (cycleResult.hasCycle) {
        return {{}, -1};  // Cannot compute critical path with cycles
    }

    // Memoization for longest path from each node
    std::unordered_map<int, int> memo;
    std::unordered_map<int, int>
        nextOnPath;  // Track the next node on longest path

    int maxLength = 0;
    int startNode = -1;

    // Find the longest path starting from any node
    for (const auto& [taskId, _] : adjacencyList_) {
        int length = dfsLongestPath(taskId, memo, nextOnPath);
        if (length > maxLength) {
            maxLength = length;
            startNode = taskId;
        }
    }

    // Reconstruct the critical path
    std::vector<int> path;
    if (startNode != -1) {
        int current = startNode;
        while (current != -1) {
            path.push_back(current);
            auto it = nextOnPath.find(current);
            if (it != nextOnPath.end()) {
                current = it->second;
            } else {
                break;
            }
        }
    }

    return {path, maxLength};
}

int TaskGraph::dfsLongestPath(int taskId, std::unordered_map<int, int>& memo,
                              std::unordered_map<int, int>& nextOnPath) const {
    // Check memo
    auto memoIt = memo.find(taskId);
    if (memoIt != memo.end()) {
        return memoIt->second;
    }

    int maxLength = 1;  // At minimum, the path includes this node
    int bestNext = -1;

    // Look at tasks that depend on this task (follow reverse edges)
    // Critical path goes: dependencies -> task -> dependents
    auto it = reverseAdjacencyList_.find(taskId);
    if (it != reverseAdjacencyList_.end()) {
        for (int dependent : it->second) {
            int length = 1 + dfsLongestPath(dependent, memo, nextOnPath);
            if (length > maxLength) {
                maxLength = length;
                bestNext = dependent;
            }
        }
    }

    memo[taskId] = maxLength;
    if (bestNext != -1) {
        nextOnPath[taskId] = bestNext;
    }

    return maxLength;
}

// ============================================================================
// Utility Methods
// ============================================================================

void TaskGraph::loadFromDependencies(
    const std::vector<TaskDependency>& dependencies) {
    for (const auto& dep : dependencies) {
        addTask(dep.taskId);
        addTask(dep.dependsOnTaskId);
        adjacencyList_[dep.taskId].insert(dep.dependsOnTaskId);
        reverseAdjacencyList_[dep.dependsOnTaskId].insert(dep.taskId);
    }
}

void TaskGraph::clear() {
    adjacencyList_.clear();
    reverseAdjacencyList_.clear();
    tasks_.clear();
}

}  // namespace core
