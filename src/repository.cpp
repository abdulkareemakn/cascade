#include "repository.h"

#include <optional>
#include <vector>

#include "PriorityQueue.h"
#include "TaskGraph.h"
#include "database.h"
#include "models.h"
#include "sorting.h"

namespace repo {
// ============================================================================
// Graph Operations (DSA + DB)
// ============================================================================
core::TaskGraph buildTaskGraph(int userId) {
    core::TaskGraph graph;

    auto user = db::getUserById(userId);
    if (!user.has_value()) {
        return graph;
    }

    std::vector<Task> tasks = db::getTasksByUser(userId);
    for (const auto& task : tasks) {
        graph.addTask(task);
    }

    for (const auto& task : tasks) {
        auto dependencies = db::getTaskDependencies(task.id);
        for (const auto& dep : dependencies) {
            graph.addDependency(task.id, dep.id);
        }
    }

    return graph;
}

bool addTaskDependencyWithValidation(int taskId, int dependsOnId) {
    auto task = db::getTask(taskId);
    if (!task) {
        return false;
    }
    auto graph = buildTaskGraph(task->ownerId);
    bool canAdd = graph.addDependency(taskId, dependsOnId);
    if (!canAdd) {
        return false;
    }
    return db::addTaskDependency(taskId, dependsOnId);
}

std::vector<int> getTaskExecutionOrder(int userId) {
    auto graph = buildTaskGraph(userId);

    std::vector<int> order = graph.topologicalSort();

    return order;
}

core::CriticalPathResult getProjectCriticalPath(int userId) {
    auto graph = buildTaskGraph(userId);
    return graph.criticalPath();
}

core::Queue loadUserTaskQueue(int userId) {
    core::Queue queue;

    auto tasks = db::getIncompleteTasksByUser(userId);

    for (const auto& task : tasks) {
        queue.insert(task);
    }

    return queue;
}

std::optional<Task> getNextPriorityTask(int userId) {
    auto queue = loadUserTaskQueue(userId);
    return queue.extractMin();
}

std::vector<Task> getSortedTasks(int userId,
                                 bool (*comparator)(const Task&, const Task&)) {
    auto tasks = db::getTasksByUser(userId);

    core::mergeSort(tasks, comparator);

    return tasks;
}
}  // namespace repo
