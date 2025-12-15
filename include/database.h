#pragma once

#include <optional>
#include <string>
#include <vector>

#include "SQLiteCpp/Database.h"
#include "models.h"

namespace db {

void initDatabase();
SQLite::Database &getConnection();


bool hasUser();
bool createUser(const std::string &username);
User getUser();

std::string getUsername(int userId);
bool updateUsername(const std::string &oldUsername,
                    const std::string &newUsername);

// TODO: Delete Database

bool createTask(const std::string &title, int priority, int status,
                std::time_t dueDate);
std::optional<Task> getTask(int id);
std::vector<Task> getTasksByUser();
std::vector<Task> getIncompleteTasksByUser();
bool updateTaskStatus(int id, int status);
bool updateTaskPriority(int id, int priority);
bool updateTaskDueDate(int id, std::time_t dueDate);
bool updateTaskTitle(int id, const std::string &title);
bool deleteTask(int id);

}  // namespace db
