# Sorting Algorithms - Cascade Project

## Overview

The Cascade project implements two O(n log n) sorting algorithms with different characteristics:
- **Quick Sort** - Average O(n log n), in-place, unstable
- **Merge Sort** - Guaranteed O(n log n), stable, uses O(n) space

Both algorithms use the **Comparator Pattern** for flexible sorting criteria.

---

## Table of Contents
1. [Comparator Pattern](#comparator-pattern)
2. [Merge Sort](#merge-sort)
3. [Quick Sort](#quick-sort)
4. [Algorithm Comparison](#algorithm-comparison)
5. [Usage Examples](#usage-examples)

---

## Comparator Pattern

### What is a Comparator?

A **comparator** is a function that defines HOW to compare two objects. Instead of hard-coding "sort by priority" into your sorting function, you pass a comparator function that defines the ordering.

### Syntax Breakdown

```cpp
bool (*comparator)(const Task&, const Task&)
```

**Breaking it down:**
- `bool` - Returns true/false
- `(*comparator)` - A **function pointer** (variable storing a function's address)
- `(const Task&, const Task&)` - Takes two Task references as parameters

**Translation:** "comparator is a pointer to a function that takes two Tasks and returns a bool"

### How It Works

```cpp
// Define a comparator function
bool byPriority(const Task& a, const Task& b) {
    return a.priority < b.priority;  // true if 'a' should come before 'b'
}

// Use it with sorting
std::vector<Task> tasks = getTasks();
quickSort(tasks, byPriority);  // Pass function by name
```

### Why Use This Pattern?

1. **Flexibility** - Same sorting code works for different orderings
2. **Reusability** - Write algorithm once, use many ways
3. **Separation of Concerns** - Sorting logic separate from comparison logic

### Provided Comparators

```cpp
// Sort by priority (lower number = higher priority)
inline bool byPriority(const Task& a, const Task& b) {
    return a.priority < b.priority;
}

// Sort by due date (earlier deadline first)
inline bool byDueDate(const Task& a, const Task& b) {
    return a.dueDate < b.dueDate;
}

// Sort by priority first, then by due date
inline bool byPriorityThenDueDate(const Task& a, const Task& b) {
    if (a.priority != b.priority) {
        return a.priority < b.priority;
    }
    return a.dueDate < b.dueDate;
}

// Sort by due date first, then by priority
inline bool byDueDateThenPriority(const Task& a, const Task& b) {
    if (a.dueDate != b.dueDate) {
        return a.dueDate < b.dueDate;
    }
    return a.priority < b.priority;
}

// Sort by task ID
inline bool byId(const Task& a, const Task& b) {
    return a.id < b.id;
}
```

---

## Merge Sort

### Algorithm Overview

**Merge Sort** is a "divide and conquer" algorithm that recursively splits the array in half until single elements remain, then merges them back in sorted order.

### Properties

| Property | Value |
|----------|-------|
| Time Complexity (Average) | O(n log n) |
| Time Complexity (Worst) | O(n log n) ✅ GUARANTEED |
| Space Complexity | O(n) |
| Stability | Stable ✅ |
| In-Place | No |

**Stability:** Preserves the relative order of equal elements. If two tasks have the same priority, their original order is maintained.

### How It Works

#### Visual Example

```
Input: [3, 1, 4, 1, 5]

DIVIDE PHASE (split until single elements):
[3, 1, 4, 1, 5]
       ↓
  [3, 1]    [4, 1, 5]
     ↓          ↓
  [3] [1]   [4] [1, 5]
                  ↓
               [4] [1] [5]

MERGE PHASE (combine sorted halves):
  [1, 3]    [1, 4, 5]
       ↓
  [1, 1, 3, 4, 5]  ✓ Sorted!
```

#### Step-by-Step Process

**Example:** Sort `[38, 27, 43, 3, 9, 82, 10]`

**Step 1: Divide**
```
[38, 27, 43, 3, 9, 82, 10]
          ↓
[38, 27, 43, 3]  [9, 82, 10]
        ↓              ↓
[38, 27] [43, 3]   [9, 82] [10]
    ↓       ↓         ↓      ↓
[38] [27] [43] [3]  [9] [82] [10]
```

**Step 2: Merge (compare and combine)**
```
[27, 38] [3, 43]   [9, 82] [10]
        ↓                ↓
[3, 27, 38, 43]    [9, 10, 82]
             ↓
[3, 9, 10, 27, 38, 43, 82]  ✓
```

### The Merge Operation

The merge step combines two sorted arrays into one:

```
Left:  [3, 27, 38]
Right: [9, 10, 43]

Compare: 3 < 9 → take 3   → [3]
Compare: 27 > 9 → take 9   → [3, 9]
Compare: 27 > 10 → take 10 → [3, 9, 10]
Compare: 27 < 43 → take 27 → [3, 9, 10, 27]
Compare: 38 < 43 → take 38 → [3, 9, 10, 27, 38]
Left empty, copy rest      → [3, 9, 10, 27, 38, 43]
```

### Maintaining Stability

The key to stability is in the merge comparison:

```cpp
// STABLE: Pick from LEFT when equal
if (!comparator(tasks[j], tasks[i])) {
    temp[k++] = tasks[i++];  // Left element
} else {
    temp[k++] = tasks[j++];  // Right element
}
```

When `tasks[i]` equals `tasks[j]`, `!comparator(tasks[j], tasks[i])` is true, so we pick from the left subarray first. This preserves the original order.

### Complexity Analysis

**Time Complexity: O(n log n)**
- **Divide:** log n levels (each level splits array in half)
- **Merge:** O(n) work per level (every element compared once)
- **Total:** O(n) × log n = O(n log n)

**Space Complexity: O(n)**
- Temporary array of size n for merging
- Recursion stack: O(log n)
- Total: O(n)

### When to Use Merge Sort

✅ **Use when:**
- You need **guaranteed** O(n log n) performance
- **Stability** is important (preserve order of equal elements)
- Working with linked lists (no random access needed)
- Parallel processing (divide-conquer naturally parallelizable)

❌ **Don't use when:**
- Memory is constrained (needs O(n) extra space)
- Working with small arrays (overhead not worth it)

---

## Quick Sort

### Algorithm Overview

**Quick Sort** is a "divide and conquer" algorithm that picks a pivot element, partitions the array around it (smaller left, larger right), then recursively sorts both partitions.

### Properties

| Property | Value |
|----------|-------|
| Time Complexity (Average) | O(n log n) ✅ |
| Time Complexity (Worst) | O(n²) ⚠️ (rare with good pivot) |
| Space Complexity | O(log n) (recursion stack) |
| Stability | Unstable ❌ |
| In-Place | Yes ✅ |

**In-Place:** Sorts the array without needing extra memory (besides recursion stack).

### How It Works

#### Visual Example

```
Input: [3, 1, 4, 1, 5]  pivot=4

PARTITION:
Move elements smaller than 4 to left, larger to right
[3, 1, 1] | 4 | [5]
 ↑ smaller  pivot  larger ↑

RECURSE:
Sort left: [1, 1, 3]
Sort right: [5] (single element, done)

RESULT: [1, 1, 3, 4, 5]  ✓
```

#### Step-by-Step Process

**Example:** Sort `[10, 7, 8, 9, 1, 5]` with pivot = 5

**Step 1: Choose pivot**
```
[10, 7, 8, 9, 1, 5]
                 ↑ pivot = 5
```

**Step 2: Partition**
```
Scan from both ends:
i→                 ←j
[10, 7, 8, 9, 1, 5]

i stops at 10 (≥5)
j stops at 1 (<5)
Swap: [1, 7, 8, 9, 10, 5]

   i→           ←j
[1, 7, 8, 9, 10, 5]

i stops at 7 (≥5)
j stops at 1 (already processed)
Swap: [1, 5, 8, 9, 10, 7]
        ↑
     final pivot position

Result: [1] | 5 | [8, 9, 10, 7]
```

**Step 3: Recursively sort partitions**
```
Left: [1] → already sorted
Right: [8, 9, 10, 7] → recurse with pivot=9

[8, 7] | 9 | [10]
  ↓
[7, 8] | 9 | [10]

Final: [1, 5, 7, 8, 9, 10]  ✓
```

### Hoare Partition Scheme

Our implementation uses the **Hoare partition scheme** (not Lomuto):

```cpp
static int partition(std::vector<Task>& tasks, int low, int high,
                     bool (*comparator)(const Task&, const Task&)) {
    // Choose middle element as pivot
    int mid = low + (high - low) / 2;
    Task pivot = tasks[mid];

    int i = low - 1;      // Left pointer
    int j = high + 1;     // Right pointer

    while (true) {
        // Move i right until element >= pivot
        do { i++; } while (comparator(tasks[i], pivot));

        // Move j left until element <= pivot
        do { j--; } while (comparator(pivot, tasks[j]));

        // If pointers crossed, partition done
        if (i >= j) return j;

        // Swap elements on wrong sides
        std::swap(tasks[i], tasks[j]);
    }
}
```

**Why Hoare over Lomuto?**
1. **Fewer swaps** - ~3x fewer on average
2. **Better cache locality** - accesses from both ends
3. **Handles duplicates better** - doesn't degrade to O(n²) easily

### Pivot Selection

**We use middle element as pivot:**
```cpp
int mid = low + (high - low) / 2;
Task pivot = tasks[mid];
```

**Why middle?**
- Avoids O(n²) worst case on already sorted data
- Better than always picking first/last element
- Simple and effective

**Other strategies:**
- Random pivot (best for avoiding worst case)
- Median-of-three (first, middle, last)

### Complexity Analysis

**Time Complexity:**

**Average Case: O(n log n)**
- Pivot splits array roughly in half
- log n levels of recursion
- O(n) work per level
- Total: O(n log n)

**Worst Case: O(n²)** - Rare!
- Happens when pivot is always smallest/largest
- Array splits as [n-1, 1] each time
- n levels × O(n) work = O(n²)
- Mitigated by good pivot selection

**Space Complexity: O(log n)**
- Recursion stack depth
- In-place sorting (no extra array)

### When to Use Quick Sort

✅ **Use when:**
- **General-purpose sorting** (most common choice)
- Memory is limited (in-place)
- Average case performance matters
- Cache efficiency important

❌ **Don't use when:**
- You need **guaranteed** O(n log n) (use Merge Sort)
- Stability required (use Merge Sort)
- Already sorted data with poor pivot selection

---

## Algorithm Comparison

### Performance Table

| Feature | Merge Sort | Quick Sort |
|---------|-----------|-----------|
| Time (Average) | O(n log n) | O(n log n) |
| Time (Worst) | O(n log n) ✅ | O(n²) ⚠️ |
| Space | O(n) | O(log n) ✅ |
| Stable | Yes ✅ | No ❌ |
| In-Place | No | Yes ✅ |
| Cache Performance | Good | Excellent ✅ |
| Parallelizable | Yes ✅ | Harder |

### Which to Choose?

**Use Merge Sort when:**
- Guaranteed O(n log n) required
- Stability needed (preserve order of equals)
- Sorting linked lists
- External sorting (data doesn't fit in memory)

**Use Quick Sort when:**
- General-purpose sorting
- Memory constrained (in-place)
- Working with arrays (random access)
- Average performance matters most

### Real-World Performance

Despite having O(n²) worst case, Quick Sort is often faster in practice because:
1. **Cache efficiency** - Better memory access patterns
2. **In-place** - No allocation overhead
3. **Low constant factors** - Fewer operations per comparison
4. **Good pivots** - Worst case rare with modern strategies

---

## Usage Examples

### Basic Usage

```cpp
#include "sorting.h"

std::vector<Task> tasks = db::getTasksByUser(userId);

// Sort by priority using Quick Sort
core::quickSort(tasks, core::byPriority);

// Sort by deadline using Merge Sort (stable)
core::mergeSort(tasks, core::byDueDate);
```

### Using Different Comparators

```cpp
// Sort by priority only
core::quickSort(tasks, core::byPriority);

// Sort by due date only
core::mergeSort(tasks, core::byDueDate);

// Sort by priority, then deadline (if priorities equal)
core::quickSort(tasks, core::byPriorityThenDueDate);

// Sort by deadline, then priority (if dates equal)
core::mergeSort(tasks, core::byDueDateThenPriority);
```

### Custom Comparators

You can write your own comparator functions:

```cpp
// Sort by title (alphabetical)
bool byTitle(const Task& a, const Task& b) {
    return a.title < b.title;
}

// Sort by owner ID
bool byOwner(const Task& a, const Task& b) {
    return a.ownerId < b.ownerId;
}

// Use them
core::quickSort(tasks, byTitle);
core::mergeSort(tasks, byOwner);
```

### When Stability Matters

```cpp
std::vector<Task> tasks = getTasks();

// First sort by deadline
core::mergeSort(tasks, core::byDueDate);

// Then stable sort by priority
// Tasks with same priority maintain deadline order!
core::mergeSort(tasks, core::byPriority);
```

**Result:** Tasks ordered by priority, but within each priority level, they're sorted by deadline (because merge sort is stable).

### Choosing the Right Algorithm

```cpp
// Need guaranteed performance? Use Merge Sort
if (criticalSystem) {
    core::mergeSort(tasks, comparator);
}

// General case? Use Quick Sort (faster on average)
else {
    core::quickSort(tasks, comparator);
}

// Need stability? Use Merge Sort
if (preserveOrderOfEquals) {
    core::mergeSort(tasks, comparator);
}
```

---

## Implementation Files

- **Header:** `include/sorting.h`
- **Implementation:** `src/sorting.cpp`
- **Tests:** `src/main.cpp` (lines 169-396)
- **Documentation:** This file

## Test Coverage

The implementation includes 6 comprehensive tests:

1. **Quick Sort by Priority** - Basic functionality
2. **Merge Sort by Due Date** - Date-based sorting
3. **Composite Sort** - Multi-criteria comparator
4. **Stability Test** - Merge Sort preserves order
5. **Edge Cases** - Empty arrays, single elements
6. **Large Dataset** - 100 tasks, both algorithms agree

All tests pass with 100% success rate.

## References

- **DSA Requirement:** DSA-07, FR-TO-01
- **Related:** Priority Queue (DSA-02) uses heap-based ordering
- **Next:** Graph Algorithms (DSA-03) for task dependencies
