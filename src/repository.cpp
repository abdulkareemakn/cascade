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

void listTasks(bool showAll, int filterStatus, int filterPriority,
               const std::string &sortBy) {
    std::vector<Task> tasks;

    // Get base task list
    if (showAll) {
        tasks = db::getTasksByUser();
    } else {
        tasks = db::getIncompleteTasksByUser();
    }

    // Filter by status if specified (-1 means no filter)
    if (filterStatus >= 0) {
        std::erase_if(tasks, [filterStatus](const Task &t) {
            return t.status != filterStatus;
        });
    }

    // Filter by priority if specified (-1 means no filter)
    if (filterPriority >= 0) {
        std::erase_if(tasks, [filterPriority](const Task &t) {
            return t.priority != filterPriority;
        });
    }

    if (sortBy == "priority") {
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

void addDependency(int taskId, int dependsOnId) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("Task {} does not exist.", taskId);
        return;
    }

    auto dependsOn = db::getTask(dependsOnId);
    if (!dependsOn.has_value()) {
        std::println("Task {} does not exist.", dependsOnId);
        return;
    }

    if (taskId == dependsOnId) {
        std::println("A task cannot depend on itself.");
        return;
    }

    if (db::wouldCreateCycle(taskId, dependsOnId)) {
        std::println(
            "Cannot add dependency: would create a circular dependency.");
        return;
    }

    if (db::addTaskDependency(taskId, dependsOnId)) {
        std::println("Task {} now depends on task {}.", taskId, dependsOnId);
    } else {
        std::println("Failed to add dependency (may already exist).");
    }
}

void removeDependency(int taskId, int dependsOnId) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("Task {} does not exist.", taskId);
        return;
    }

    if (db::removeTaskDependency(taskId, dependsOnId)) {
        std::println(
            "Removed dependency: task {} no longer depends on task {}.", taskId,
            dependsOnId);
    } else {
        std::println("Failed to remove dependency (may not exist).");
    }
}

void showDependencies(int taskId) {
    auto task = db::getTask(taskId);
    if (!task.has_value()) {
        std::println("Task {} does not exist.", taskId);
        return;
    }

    auto dependencies = db::getTaskDependencies(taskId);
    auto dependents = db::getTaskDependents(taskId);

    std::println("Task {}: {}", taskId, task->title);
    std::println("");

    if (dependencies.empty()) {
        std::println("Dependencies (tasks this depends on): None");
    } else {
        std::println("Dependencies (tasks this depends on):");
        tabulate::Table depTable;
        depTable.add_row({"ID", "Title", "Status"});
        for (const auto &dep : dependencies) {
            depTable.add_row(tabulate::RowStream{}
                             << dep.id << dep.title
                             << statusToString(dep.status));
        }
        printStyledTable(depTable);
    }

    if (dependents.empty()) {
        std::println("Dependents (tasks that depend on this): None");
    } else {
        std::println("Dependents (tasks that depend on this):");
        tabulate::Table deptTable;
        deptTable.add_row({"ID", "Title", "Status"});
        for (const auto &dept : dependents) {
            deptTable.add_row(tabulate::RowStream{}
                              << dept.id << dept.title
                              << statusToString(dept.status));
        }
        printStyledTable(deptTable);
    }
}

void showExecutionPlan() {
    auto order = db::getTopologicalOrder();

    if (order.empty()) {
        std::println("No tasks to plan.");
        return;
    }

    std::println("Execution Plan (Topological Sort)");
    std::println("==================================");
    std::println("Complete tasks in this order to respect all dependencies:\n");

    tabulate::Table table;
    table.add_row({"Step", "ID", "Title", "Priority", "Status"});

    int step = 1;
    for (int taskId : order) {
        auto task = db::getTask(taskId);
        if (task.has_value()) {
            table.add_row(tabulate::RowStream{}
                          << step++ << task->id << task->title << task->priority
                          << statusToString(task->status));
        }
    }
    printStyledTable(table);
}

void showCriticalPath() {
    auto path = db::getCriticalPath();

    if (path.empty()) {
        std::println("No critical path found (no dependencies between tasks).");
        return;
    }

    std::println("Critical Path Analysis");
    std::println("======================");
    std::println("The longest dependency chain has {} tasks:\n", path.size());

    tabulate::Table table;
    table.add_row({"Order", "ID", "Title", "Priority", "Status"});

    int order = 1;
    for (int taskId : path) {
        auto task = db::getTask(taskId);
        if (task.has_value()) {
            table.add_row(tabulate::RowStream{}
                          << order++ << task->id << task->title
                          << task->priority << statusToString(task->status));
        }
    }
    printStyledTable(table);

    std::println(
        "\nThis chain represents the minimum number of sequential steps");
    std::println("required to complete all dependent tasks.");
}
}  // namespace repo