#include "sorting.h"

#include <vector>

namespace core {

static void merge(std::vector<Task>& tasks, int left, int mid, int right,
                  bool (*comparator)(const Task&, const Task&),
                  std::vector<Task>& temp) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (!comparator(tasks[j], tasks[i])) {
            temp[k++] = tasks[i++];
        } else {
            temp[k++] = tasks[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = tasks[i++];
    }

    while (j <= right) {
        temp[k++] = tasks[j++];
    }

    for (i = left; i <= right; i++) {
        tasks[i] = temp[i];
    }
}

static void mergeSortHelper(std::vector<Task>& tasks, int left, int right,
                            bool (*comparator)(const Task&, const Task&),
                            std::vector<Task>& temp) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    mergeSortHelper(tasks, left, mid, comparator, temp);
    mergeSortHelper(tasks, mid + 1, right, comparator, temp);
    merge(tasks, left, mid, right, comparator, temp);
}

void mergeSort(std::vector<Task>& tasks,
               bool (*comparator)(const Task&, const Task&)) {
    if (tasks.size() <= 1) {
        return;
    }

    std::vector<Task> temp(tasks.size());
    mergeSortHelper(tasks, 0, static_cast<int>(tasks.size()) - 1, comparator,
                    temp);
}

}  // namespace core