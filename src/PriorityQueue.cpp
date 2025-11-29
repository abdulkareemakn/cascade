#include "PriorityQueue.h"

#include <optional>
#include <utility>
#include <vector>

#include "models.h"

void core::Queue::insert(Task task) {
    heap.emplace_back(task);
    heapifyUp(heap.size() - 1);
}

std::optional<Task> core::Queue::extractMin() {
    if (isEmpty()) {
        return std::nullopt;
    }

    auto minTask = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap.pop_back();

    if (!isEmpty()) {
        heapifyDown(0);
    }

    return minTask;
}

std::optional<Task> core::Queue::peek() {
    if (core::Queue::isEmpty()) {
        return std::nullopt;
    }
    return heap[0];
}

bool core::Queue::isEmpty() { return heap.empty(); }

int core::Queue::getSize() { return heap.size(); }

int core::Queue::parent(int i) { return (i - 1) / 2; }
int core::Queue::leftChild(int i) { return 2 * i + 1; }
int core::Queue::rightChild(int i) { return 2 * i + 2; }

bool core::Queue::isHigherPriority(const Task& a, const Task& b) {
    if (a.priority > b.priority) {
        return false;
    }
    if (a.priority < b.priority) {
        return true;
    }

    return a.dueDate < b.dueDate;
}

void core::Queue::heapifyUp(int index) {
    while (index > 0) {
        int parent_index = parent(index);
        if (isHigherPriority(heap[index], heap[parent_index])) {
            std::swap(heap[index], heap[parent_index]);
            index = parent_index;

        } else {
            break;
        }
    }
}

void core::Queue::heapifyDown(int index) {
    while (leftChild(index) < heap.size()) {
        auto smallerChild = leftChild(index);
        auto rightIndex = rightChild(index);

        if (rightIndex < heap.size()) {
            if (isHigherPriority(heap[rightIndex], heap[smallerChild])) {
                smallerChild = rightIndex;
            }
        }
        if (isHigherPriority(heap[smallerChild], heap[index])) {
            std::swap(heap[index], heap[smallerChild]);
            index = smallerChild;

        } else {
            break;
        }
    }
}
