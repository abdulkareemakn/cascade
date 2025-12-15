#pragma once

#include <string>

struct UserArgs {
    int id;
    std::string username;
};

struct TaskArgs {
    std::string title;
    int priority = 2;
    std::string dueDate = "0";
    std::string status = "0";
    std::string sortBy;
    bool showAll = false;
    int taskId = 0;
    int filterStatus = -1;    // -1 = no filter
    int filterPriority = -1;  // -1 = no filter
    int updatePriority = -1;  // -1 = not set
    std::string updateStatus;
    std::string updateDueDate;
    std::string updateTitle;
};

struct DepsArgs {
    int taskId = 0;
    int dependsOnId = 0;
};

struct CommandArgs {
    UserArgs user;
    TaskArgs task;
    DepsArgs deps;
};
