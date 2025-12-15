#pragma once

#include <optional>
#include <vector>

#include "models.h"

namespace core {
class Queue {
   public:
    void insert(Task task);
    std::optional<Task> extractMin();
    std::optional<Task> peek();
    bool isEmpty();
    int getSize();

   private:
    std::vector<Task> tasks;

    bool isHigherPriority(const Task& a, const Task& b);
};

}  // namespace core