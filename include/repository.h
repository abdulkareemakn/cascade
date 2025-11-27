#pragma once

#include <optional>
#include <string>
#include <vector>

#include "models.h"
namespace db {
bool createUser(const std::string& username, const std::string& email,
                const std::string& password);

std::optional<User> getUser(const std::string& username);

bool createTask(const std::string& title, int priority, std::time_t dueDate,
                int ownerId);

std::vector<Task> getTasks(const std::string& username);
}  // namespace db
