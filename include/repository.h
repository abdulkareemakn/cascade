#pragma once

#include <string>
namespace db {
bool createUser(const std::string& username, const std::string& email,
                const std::string& password);
void getUser();

bool createTask();
void getTasks();
}  // namespace db
