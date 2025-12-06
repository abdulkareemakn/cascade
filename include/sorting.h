#pragma once

#include <vector>

#include "models.h"

namespace core {

// Sorting Algorithms
void mergeSort(std::vector<Task>& tasks,
               bool (*comparator)(const Task&, const Task&));

void quickSort(std::vector<Task>& tasks,
               bool (*comparator)(const Task&, const Task&));

// Comparator Functions (return true if 'a' comes before 'b')
inline bool byPriority(const Task& a, const Task& b) {
    return a.priority < b.priority;
}

inline bool byDueDate(const Task& a, const Task& b) {
    return a.dueDate < b.dueDate;
}

inline bool byPriorityThenDueDate(const Task& a, const Task& b) {
    if (a.priority != b.priority) {
        return a.priority < b.priority;
    }
    return a.dueDate < b.dueDate;
}

inline bool byDueDateThenPriority(const Task& a, const Task& b) {
    if (a.dueDate != b.dueDate) {
        return a.dueDate < b.dueDate;
    }
    return a.priority < b.priority;
}

inline bool byId(const Task& a, const Task& b) { return a.id < b.id; }

}  // namespace core
