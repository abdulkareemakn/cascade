#include "repository.h"

#include <iostream>
#include <optional>
#include <print>
#include <vector>

#include "PriorityQueue.h"
#include "commands.h"
#include "database.h"
#include "models.h"
#include "sorting.h"
#include "tabulate.hpp"
#include "util.h"

namespace repo {
core::Queue loadUserTaskQueue() {
    core::Queue queue;
    auto tasks = db::getIncompleteTasksByUser();

    for (const auto &task : tasks) {
        queue.insert(task);
    }

    return queue;
}

void getNextPriorityTask() {
    auto queue = loadUserTaskQueue();
    auto taskOpt = queue.extractMin();

    if (!taskOpt.has_value()) {
        std::println("No incomplete tasks found.");
        return;
    }

    Task task = taskOpt.value();
    tabulate::Table table;
    table.add_row({"ID", "Title", "Priority", "Status", "Due Date"});
    table.add_row(tabulate::RowStream{}
                  << task.id << task.title << task.priority
                  << statusToString(task.status) << formatDate(task.dueDate));
    printStyledTable(table);
}

std::vector<Task> getSortedTasks(bool (*comparator)(const Task &,
                                                    const Task &)) {
    auto tasks = db::getTasksByUser();
    core::mergeSort(tasks, comparator);
    return tasks;
}

void showTask(int taskId) {
    auto optional = db::getTask(taskId);

    if (!optional.has_value()) {
        std::println("No task with that id.");
    } else {
        Task task = optional.value();
        tabulate::Table table;
        table.add_row({"ID", "Title", "Priority", "Status", "Due Date"});
        table.add_row(tabulate::RowStream{} << task.id << task.title
                                            << task.priority
                                            << statusToString(task.status)
                                            << formatDate(task.dueDate));
        printStyledTable(table);
    }
}

void showAllTasks() {
    auto tasks = db::getTasksByUser();
    if (tasks.empty()) {
        std::println("No tasks found.");
        return;
    }

    tabulate::Table table;
    table.add_row({"ID", "Title", "Priority", "Status", "Due Date"});

    for (const auto &task : tasks) {
        table.add_row(tabulate::RowStream{} << task.id << task.title
                                            << task.priority
                                            << statusToString(task.status)
                                            << formatDate(task.dueDate));
    }
    printStyledTable(table);
}

void listTasks(bool showAll, bool sortByStatus, bool sortByPriority,
               const std::string &sortBy) {
    std::vector<Task> tasks;

    // Get base task list
    if (showAll) {
        tasks = db::getTasksByUser();
    } else {
        tasks = db::getIncompleteTasksByUser();
    }

    // Sort by priority flag (takes precedence over --sort)
    if (sortByPriority) {
        core::mergeSort(tasks, core::byPriority);
    }
    // Sort by status flag (takes precedence over --sort)
    else if (sortByStatus) {
        core::mergeSort(tasks, core::byStatus);
    }
    // Sort by explicit --sort option
    else if (sortBy == "priority") {
        core::mergeSort(tasks, core::byPriority);
    } else if (sortBy == "date") {
        core::mergeSort(tasks, core::byDueDate);
    } else if (sortBy == "created") {
        core::mergeSort(tasks, core::byId);
    }

    if (tasks.empty()) {
        std::println("No tasks found.");
        return;
    }

    tabulate::Table table;
    table.add_row({"ID", "Title", "Priority", "Status", "Due Date"});

    for (const auto &task : tasks) {
        table.add_row(tabulate::RowStream{} << task.id << task.title
                                            << task.priority
                                            << statusToString(task.status)
                                            << formatDate(task.dueDate));
    }
    printStyledTable(table);
}

void updateTaskPriority(int taskId, int priority) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("No task with id {}.", taskId);
        return;
    }

    if (priority < 1 || priority > 4) {
        std::println("Priority must be between 1 and 4.");
        return;
    }

    if (db::updateTaskPriority(taskId, priority)) {
        std::println("Updated task {} priority to {}.", taskId, priority);
    } else {
        std::println("Failed to update task priority.");
    }
}

void updateTaskStatus(int taskId, int status) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("No task with id {}.", taskId);
        return;
    }

    if (status < 0 || status > 3) {
        std::println(
            "Invalid status. Use: 0=TODO, 1=IN_PROGRESS, 2=COMPLETE, "
            "3=WONT_DO");
        return;
    }

    if (db::updateTaskStatus(taskId, status)) {
        std::println("Updated task {} status.", taskId);
    } else {
        std::println("Failed to update task status.");
    }
}

void updateTaskDueDate(int taskId, std::time_t dueDate) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("No task with id {}.", taskId);
        return;
    }

    if (db::updateTaskDueDate(taskId, dueDate)) {
        std::println("Updated task {} due date.", taskId);
    } else {
        std::println("Failed to update task due date.");
    }
}

void updateTaskTitle(int taskId, const std::string &title) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("No task with id {}.", taskId);
        return;
    }

    if (title.empty()) {
        std::println("Title cannot be empty.");
        return;
    }

    if (db::updateTaskTitle(taskId, title)) {
        std::println("Updated task {} title to \"{}\".", taskId, title);
    } else {
        std::println("Failed to update task title.");
    }
}

void deleteTask(int taskId) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("No task with id {}.", taskId);
        return;
    }

    if (db::deleteTask(taskId)) {
        std::println("Deleted task {}.", taskId);
    } else {
        std::println("Failed to delete task.");
    }
}
}  // namespace repo