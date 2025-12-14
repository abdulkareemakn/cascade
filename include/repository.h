#pragma once

#include <optional>
#include <string>
#include <vector>

#include "PriorityQueue.h"
#include "TaskGraph.h"
#include "commands.h"
#include "models.h"
#include "sorting.h"

// Graph Operations (DSA + DB)

namespace repo {
core::TaskGraph buildTaskGraph();
bool addTaskDependencyWithValidation(int taskId, int dependsOnId);
std::vector<int> getTaskExecutionOrder();
core::CriticalPathResult getProjectCriticalPath();

// Priority Queue Operations (DSA + DB)
core::Queue loadUserTaskQueue();
void getNextPriorityTask();

// Sorting Operations (DSA + DB)
std::vector<Task> getSortedTasks(bool (*comparator)(const Task &,
                                                    const Task &));

// Display Operations
void showTask(int taskId);
void showAllTasks();
void listTasks(bool showAll, int filterStatus, int filterPriority,
               const std::string &sortBy);

// Update Operations
void updateTaskPriority(int taskId, int priority);
void updateTaskStatus(int taskId, int status);
void updateTaskDueDate(int taskId, std::time_t dueDate);
void updateTaskTitle(int taskId, const std::string &title);
void deleteTask(int taskId);

// Dependency Operations (Graph DSA)
void addDependency(int taskId, int dependsOnId);
void removeDependency(int taskId, int dependsOnId);
void showDependencies(int taskId);
void showExecutionPlan();
void showCriticalPath();
}  // namespace repo
