#include "PriorityQueue.h"

#include <algorithm>
#include <optional>
#include <vector>

#include "models.h"

void core::Queue::insert(Task task) {
    tasks.emplace_back(task);
}

std::optional<Task> core::Queue::extractMin() {
    if (isEmpty()) {
        return std::nullopt;
    }

    int minIndex = 0;
    for (int i = 1; i < tasks.size(); i++) {
        if (isHigherPriority(tasks[i], tasks[minIndex])) {
            minIndex = i;
        }
    }

    auto minTask = tasks[minIndex];
    tasks.erase(tasks.begin() + minIndex);

    return minTask;
}

std::optional<Task> core::Queue::peek() {
    if (isEmpty()) {
        return std::nullopt;
    }

    int minIndex = 0;
    for (int i = 1; i < tasks.size(); i++) {
        if (isHigherPriority(tasks[i], tasks[minIndex])) {
            minIndex = i;
        }
    }

    return tasks[minIndex];
}

bool core::Queue::isEmpty() { return tasks.empty(); }

int core::Queue::getSize() { return tasks.size(); }

bool core::Queue::isHigherPriority(const Task& a, const Task& b) {
    if (a.priority > b.priority) {
        return false;
    }
    if (a.priority < b.priority) {
        return true;
    }

    return a.dueDate < b.dueDate;
}