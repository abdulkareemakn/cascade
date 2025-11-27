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
    int id;
    std::string title;
    int priority;
    std::time_t dueDate;
    std::time_t creationTime;
    int ownerId;
    std::string assignedToUsername;
};
