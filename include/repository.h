#pragma once

#include <optional>
#include <string>
#include <vector>

#include "PriorityQueue.h"
#include "TaskGraph.h"
#include "models.h"
#include "sorting.h"

// Graph Operations (DSA + DB)

namespace repo {
core::TaskGraph buildTaskGraph(int userId);
bool addTaskDependencyWithValidation(int taskId, int dependsOnId);
std::vector<int> getTaskExecutionOrder(int userId);
core::CriticalPathResult getProjectCriticalPath(int userId);

// Priority Queue Operations (DSA + DB)
core::Queue loadUserTaskQueue(int userId);
std::optional<Task> getNextPriorityTask(int userId);

// Sorting Operations (DSA + DB)
std::vector<Task> getSortedTasks(int userId,
                                 bool (*comparator)(const Task&, const Task&));
}  // namespace repo
