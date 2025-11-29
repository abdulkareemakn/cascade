#pragma once

// NOTE: Use enums for status and priority if needed.

#include <optional>
#include <string>
#include <vector>

#include "SQLiteCpp/Database.h"
#include "models.h"

namespace db {

void initDatabase();
SQLite::Database& getConnection();

bool createUser(const std::string& username, const std::string& email,
                const std::string& password);
bool authenticateUser(const std::string& email, const std::string& password);
std::optional<User> getUser(const std::string& username);
std::optional<User> getUserById(int id);
std::optional<User> getUserByEmail(const std::string& email);
bool deleteUser(int id);
bool updateEmail(int userId, const std::string& newEmail);
bool updatePassword(int userId, const std::string& newPassword);
bool updateUsername(int userId, const std::string& newUsername);

bool createTask(const std::string& title, int priority, int status,
                std::time_t dueDate, int ownerId,
                const std::string& assignedToUsername = "");
std::optional<Task> getTask(int id);
std::vector<Task> getTasksByUser(int userId);
std::vector<Task> getTasksAssignedTo(const std::string& username);
bool updateTaskStatus(int id, int status);
bool updateTaskPriority(int id, int priority);
bool updateTaskDueDate(int id, std::time_t dueDate);
bool updateTaskTitle(int id, const std::string& title);
bool assignTask(int id, const std::string& username);
bool deleteTask(int id);

}  // namespace db
