#pragma once

#include <optional>
#include <string>
#include <vector>

#include "PriorityQueue.h"
#include "commands.h"
#include "models.h"
#include "sorting.h"


namespace repo {
core::Queue loadUserTaskQueue();
void getNextPriorityTask();

std::vector<Task> getSortedTasks(bool (*comparator)(const Task &,
                                                    const Task &));

void showTask(int taskId);
void showAllTasks();
void listTasks(bool showAll, int filterStatus, int filterPriority,
               const std::string &sortBy);

void updateTaskPriority(int taskId, int priority);
void updateTaskStatus(int taskId, int status);
void updateTaskDueDate(int taskId, std::time_t dueDate);
void updateTaskTitle(int taskId, const std::string &title);
void deleteTask(int taskId);

}  // namespace repo