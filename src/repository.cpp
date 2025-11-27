#include "repository.h"

#include <ctime>
#include <optional>
#include <string>
#include <vector>

#include "database.h"
#include "models.h"
#include "security.h"

// Do not use 'using namespace sqlite_orm;' as per user request.

bool db::createUser(const std::string& username, const std::string& email,
                    const std::string& password) {
    try {
        auto storage = getStorage();
        // Check if user already exists to avoid constraint violation exception
        // (Optional, but cleaner)
        auto count = storage.count<User>(
            sqlite_orm::where(sqlite_orm::c(&User::username) == username));
        if (count > 0) return false;

        std::string hash = security::hashPassword(password);

        User user{.username = username, .email = email, .passwordHash = hash};
        auto insertedId = storage.insert(user);
        return insertedId > 0;
    } catch (...) {
        return false;
    }
}

std::optional<User> db::getUser(const std::string& username) {
    auto storage = getStorage();
    auto users = storage.get_all<User>(
        sqlite_orm::where(sqlite_orm::c(&User::username) == username));
    
    if (users.empty()) {
        return std::nullopt;
    }
    return users.front();
}

// Note: Removed assignedToUsername to match header. 
// If you need it, update header first.
bool db::createTask(const std::string& title, int priority, std::time_t dueDate,
                    int ownerId) {
    try {
        auto storage = getStorage();
        Task task{
            .title = title,
            .priority = priority,
            .dueDate = dueDate,
            .ownerId = ownerId,
            .assignedToUsername = "" // Default empty for now
        };

        auto insertedId = storage.insert(task);
        return insertedId > 0;
    } catch (...) {
        return false;
    }
}

std::vector<Task> db::getTasks(const std::string& username) {
    auto userOpt = getUser(username);
    if (!userOpt) {
        return {}; // Return empty list if user doesn't exist
    }

    auto storage = getStorage();
    return storage.get_all<Task>(
        sqlite_orm::where(sqlite_orm::c(&Task::ownerId) == userOpt->id));
}