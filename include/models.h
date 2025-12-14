#pragma once
#include <sqlite3.h>

#include <ctime>
#include <string>

struct User {
    std::string username;
    std::time_t creationTime = 0;
};

struct Task {
    int id = 0;
    std::string title;
    int priority = 2;
    int status = -1;
    std::time_t dueDate = 0;
    std::time_t creationTime = 0;
};

enum class TaskStatus : int {
    TODO = 0,
    IN_PROGRESS = 1,
    COMPLETE = 2,
    WONT_DO = 3
};

struct TaskDependency {
    int taskId;
    int dependsOnTaskId;
};
