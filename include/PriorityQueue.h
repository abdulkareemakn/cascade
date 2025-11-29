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
    std::vector<Task> heap;

    static int parent(int i);
    static int leftChild(int i);
    static int rightChild(int i);

    bool isHigherPriority(const Task& a, const Task& b);

    void heapifyUp(int index);
    void heapifyDown(int index);
};

}  // namespace core
