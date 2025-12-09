#pragma once
#include <sqlite3.h>

#include <ctime>
#include <string>

struct User {
    int id = 0;
    std::string username;
    std::string email;
    std::string passwordHash;
    std::time_t creationTime = std::time(nullptr);
};

struct Task {
    int id = 0;
    std::string title;
    int priority;
    int status;
    std::time_t dueDate;
    std::time_t creationTime = std::time(nullptr);
    int ownerId;
    std::string assignedToUsername;
};

enum class TaskStatus : int {
    TODO = 0,
    IN_PROGRESS = 1,
    DONE = 2,
    WONT_DO = 3
};

struct Team {
    int id = 0;
    std::string name;
    int creatorId;
    std::time_t creationTime = std::time(nullptr);
};

struct TeamMember {
    int userId;
    int teamId;
    std::string role;  // e.g., "admin", "member"
};

struct TaskDependency {
    int taskId;
    int dependsOnTaskId;
};
