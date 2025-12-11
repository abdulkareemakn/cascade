#include "database.h"

#include <exception>
#include <optional>
#include <print>
#include <string>
#include <vector>

#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "security.h"

void db::initDatabase() {
    SQLite::Database db(DATABASE_FILE,
                        SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    db.exec("PRAGMA foreign_keys = ON;");

    db.exec(
        "CREATE TABLE IF NOT EXISTS users (id INTEGER NOT NULL PRIMARY KEY "
        "AUTOINCREMENT, "
        "username TEXT NOT NULL, email TEXT UNIQUE NOT NULL, passwordHash TEXT "
        "NOT "
        "NULL, creationTime INTEGER NOT NULL);");

    db.exec(
        "CREATE TABLE IF NOT EXISTS tasks (id INTEGER NOT NULL PRIMARY KEY "
        "AUTOINCREMENT, "
        "title TEXT NOT NULL, priority INTEGER, status INTEGER NOT NULL, "
        "dueDate INTEGER, creationTime INTEGER NOT NULL, ownerId INTEGER NOT "
        "NULL, "
        "FOREIGN KEY (ownerId) REFERENCES users(id) ON DELETE CASCADE);");

    db.exec(
        "CREATE TABLE IF NOT EXISTS task_dependencies ("
        "taskId INTEGER NOT NULL, "
        "dependsOnTaskId INTEGER NOT NULL, "
        "FOREIGN KEY (taskId) REFERENCES tasks(id) ON DELETE CASCADE, "
        "FOREIGN KEY (dependsOnTaskId) REFERENCES tasks(id) ON DELETE CASCADE, "
        "PRIMARY KEY (taskId, dependsOnTaskId));");
}

SQLite::Database &db::getConnection() {
    // TODO: Replace singleton with connection pool for multi-user support
    static SQLite::Database db(DATABASE_FILE,
                               SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    static bool initialized = false;
    if (!initialized) {
        db.exec("PRAGMA foreign_keys = ON;");
        initialized = true;
    }
    return db;
}

bool db::createUser(const std::string &username, const std::string &email,
                    const std::string &password) {
    auto &db = db::getConnection();

    std::string hash = security::hashPassword(password);

    try {
        SQLite::Statement insert(
            db,
            "INSERT INTO users (username, email, passwordHash, creationTime) "
            "VALUES (?, ?, ?, unixepoch())");

        insert.bind(1, username);
        insert.bind(2, email);
        insert.bind(3, hash);

        insert.exec();
    }

    catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
    return true;
}

std::optional<User> db::getUser(const std::string &username) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement select(db, "SELECT * FROM users WHERE username = ?");
        select.bind(1, username);

        if (select.executeStep()) {
            User user;
            user.id = select.getColumn(0).getInt();
            user.username = select.getColumn(1).getString();
            user.email = select.getColumn(2).getString();
            user.passwordHash = select.getColumn(3).getString();
            user.creationTime = select.getColumn(4).getInt();
            return user;
        }
        return std::nullopt;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

std::optional<User> db::getUserById(int id) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement select(db, "SELECT * FROM users WHERE id = ?");
        select.bind(1, id);

        if (select.executeStep()) {
            User user;
            user.id = select.getColumn(0).getInt();
            user.username = select.getColumn(1).getString();
            user.email = select.getColumn(2).getString();
            user.passwordHash = select.getColumn(3).getString();
            user.creationTime = select.getColumn(4).getInt();
            return user;
        }
        return std::nullopt;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

std::optional<User> db::getUserByEmail(const std::string &email) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement select(db, "SELECT * FROM users WHERE email = ?");
        select.bind(1, email);

        if (select.executeStep()) {
            User user;
            user.id = select.getColumn(0).getInt();
            user.username = select.getColumn(1).getString();
            user.email = select.getColumn(2).getString();
            user.passwordHash = select.getColumn(3).getString();
            user.creationTime = select.getColumn(4).getInt();
            return user;
        }
        return std::nullopt;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

bool db::deleteUser(int id) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement del(db, "DELETE FROM users WHERE id = ?");
        del.bind(1, id);
        del.exec();

    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }

    return true;
}

std::string db::getUsername(int userId) {
    auto user = getUserById(userId);
    return user.value().username;
}

// TODO: Refactor such that getUser when not logged in calls authenticateUser
// before returning.
bool db::authenticateUser(const std::string &email,
                          const std::string &password) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement select(db, "SELECT * FROM users WHERE email = ?");
        select.bind(1, email);
        if (select.executeStep()) {
            User user;
            user.id = select.getColumn(0).getInt();
            user.username = select.getColumn(1).getString();
            user.email = select.getColumn(2).getString();
            user.passwordHash = select.getColumn(3).getString();
            user.creationTime = select.getColumn(4).getInt();
            if (security::verifyPassword(password, user.passwordHash)) {
                return true;
            }
        }
        return false;

    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updateEmail(int userId, const std::string &newEmail) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(db, "UPDATE users SET email = ? WHERE id = ?");
        update.bind(1, newEmail);
        update.bind(2, userId);
        update.exec();

        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updatePassword(int userId, const std::string &newPassword) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(
            db, "UPDATE users SET passwordHash = ? WHERE id = ?");
        update.bind(1, security::hashPassword(newPassword));
        update.bind(2, userId);
        update.exec();

        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updateUsername(int userId, const std::string &newUsername) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(db,
                                 "UPDATE users SET username = ? WHERE id = ?");
        update.bind(1, newUsername);
        update.bind(2, userId);
        update.exec();

        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::createTask(const std::string &title, int priority, int status,
                    std::time_t dueDate) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement insert(db,
                                 "INSERT INTO tasks (title, priority, status, "
                                 "dueDate, creationTime) VALUES (?, ?, "
                                 "?, ?, unixepoch(), ?)");

        insert.bind(1, title);
        insert.bind(2, priority);
        insert.bind(3, status);
        insert.bind(4, static_cast<int64_t>(dueDate));

        insert.exec();
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
    return true;
}

std::optional<Task> db::getTask(int id) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement select(db, "SELECT * FROM tasks WHERE id = ?");
        select.bind(1, id);

        if (select.executeStep()) {
            Task task;
            task.id = select.getColumn(0).getInt();
            task.title = select.getColumn(1).getString();
            task.priority = select.getColumn(2).getInt();
            task.status = select.getColumn(3).getInt();
            task.dueDate = select.getColumn(4).getInt64();
            task.creationTime = select.getColumn(5).getInt64();
            task.ownerId = select.getColumn(6).getInt();
            return task;
        }
        return std::nullopt;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

std::vector<Task> db::getTasksByUser(int userId) {
    auto &db = db::getConnection();
    std::vector<Task> tasks;

    try {
        SQLite::Statement select(db, "SELECT * FROM tasks WHERE ownerId = ?");
        select.bind(1, userId);

        while (select.executeStep()) {
            Task task;
            task.id = select.getColumn(0).getInt();
            task.title = select.getColumn(1).getString();
            task.priority = select.getColumn(2).getInt();
            task.status = select.getColumn(3).getInt();
            task.dueDate = select.getColumn(4).getInt64();
            task.creationTime = select.getColumn(5).getInt64();
            task.ownerId = select.getColumn(6).getInt();
            tasks.push_back(task);
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
    }
    return tasks;
}

std::vector<Task> db::getIncompleteTasksByUser(int userId) {
    auto &db = db::getConnection();
    std::vector<Task> tasks;

    try {
        SQLite::Statement select(
            db, "SELECT * FROM tasks WHERE ownerId = ? AND status != 2");
        select.bind(1, userId);

        while (select.executeStep()) {
            Task task;
            task.id = select.getColumn(0).getInt();
            task.title = select.getColumn(1).getString();
            task.priority = select.getColumn(2).getInt();
            task.status = select.getColumn(3).getInt();
            task.dueDate = select.getColumn(4).getInt64();
            task.creationTime = select.getColumn(5).getInt64();
            task.ownerId = select.getColumn(6).getInt();
            tasks.push_back(task);
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
    }
    return tasks;
}

bool db::updateTaskStatus(int id, int status) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(db,
                                 "UPDATE tasks SET status = ? WHERE id = ?");
        update.bind(1, status);
        update.bind(2, id);
        update.exec();
        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updateTaskPriority(int id, int priority) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(db,
                                 "UPDATE tasks SET priority = ? WHERE id = ?");
        update.bind(1, priority);
        update.bind(2, id);
        update.exec();
        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updateTaskDueDate(int id, std::time_t dueDate) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(db,
                                 "UPDATE tasks SET dueDate = ? WHERE id = ?");
        update.bind(1, static_cast<int64_t>(dueDate));
        update.bind(2, id);
        update.exec();
        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updateTaskTitle(int id, const std::string &title) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(db, "UPDATE tasks SET title = ? WHERE id = ?");
        update.bind(1, title);
        update.bind(2, id);
        update.exec();
        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::deleteTask(int id) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement del(db, "DELETE FROM tasks WHERE id = ?");
        del.bind(1, id);
        del.exec();
        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::addTaskDependency(int taskId, int dependsOnTaskId) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement insert(
            db,
            "INSERT INTO task_dependencies (taskId, dependsOnTaskId) "
            "VALUES (?, ?)");
        insert.bind(1, taskId);
        insert.bind(2, dependsOnTaskId);
        insert.exec();
        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::removeTaskDependency(int taskId, int dependsOnTaskId) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement del(db,
                              "DELETE FROM task_dependencies "
                              "WHERE taskId = ? AND dependsOnTaskId = ?");
        del.bind(1, taskId);
        del.bind(2, dependsOnTaskId);
        del.exec();
        return true;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
}

std::vector<Task> db::getTaskDependencies(int taskId) {
    auto &db = db::getConnection();
    std::vector<Task> dependencies;

    try {
        SQLite::Statement select(
            db,
            "SELECT t.* FROM tasks t "
            "JOIN task_dependencies td ON t.id = td.dependsOnTaskId "
            "WHERE td.taskId = ?");
        select.bind(1, taskId);

        while (select.executeStep()) {
            Task task;
            task.id = select.getColumn(0).getInt();
            task.title = select.getColumn(1).getString();
            task.priority = select.getColumn(2).getInt();
            task.status = select.getColumn(3).getInt();
            task.dueDate = select.getColumn(4).getInt64();
            task.creationTime = select.getColumn(5).getInt64();
            task.ownerId = select.getColumn(6).getInt();
            dependencies.push_back(task);
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
    }
    return dependencies;
}

std::vector<Task> db::getTaskDependents(int taskId) {
    auto &db = db::getConnection();
    std::vector<Task> dependents;

    try {
        SQLite::Statement select(
            db,
            "SELECT t.* FROM tasks t "
            "JOIN task_dependencies td ON t.id = td.taskId "
            "WHERE td.dependsOnTaskId = ?");
        select.bind(1, taskId);

        while (select.executeStep()) {
            Task task;
            task.id = select.getColumn(0).getInt();
            task.title = select.getColumn(1).getString();
            task.priority = select.getColumn(2).getInt();
            task.status = select.getColumn(3).getInt();
            task.dueDate = select.getColumn(4).getInt64();
            task.creationTime = select.getColumn(5).getInt64();
            task.ownerId = select.getColumn(6).getInt();
            dependents.push_back(task);
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
    }
    return dependents;
}
