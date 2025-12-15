#include "database.h"

#include <cmath>
#include <exception>
#include <optional>
#include <print>
#include <string>
#include <vector>

#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "models.h"

namespace {
Task taskFromRow(SQLite::Statement &stmt) {
    Task task;
    task.id = stmt.getColumn(0).getInt();
    task.title = stmt.getColumn(1).getString();
    task.priority = stmt.getColumn(2).getInt();
    task.status = stmt.getColumn(3).getInt();
    task.dueDate = stmt.getColumn(4).getInt64();
    task.creationTime = stmt.getColumn(5).getInt64();
    return task;
}
}  // namespace

void db::initDatabase() {
    SQLite::Database db(DATABASE_FILE,
                        SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    db.exec("PRAGMA foreign_keys = ON;");

    db.exec(
        "CREATE TABLE IF NOT EXISTS user ("
        "username TEXT NOT NULL, "
        "creationTime INTEGER NOT NULL);");

    db.exec(
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "priority INTEGER NOT NULL DEFAULT 2, "
        "status INTEGER NOT NULL DEFAULT 0, "
        "dueDate INTEGER NOT NULL DEFAULT 0, "
        "creationTime INTEGER NOT NULL);");


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

bool db::hasUser() {
    auto &db = db::getConnection();
    SQLite::Statement select(db, "SELECT COUNT(*) FROM user");
    select.executeStep();
    return select.getColumn(0).getInt() > 0;
}

bool db::createUser(const std::string &username) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement insert(db,
                                 "INSERT INTO user (username, creationTime) "
                                 "VALUES (?, unixepoch())");
        insert.bind(1, username);
        insert.exec();
    }

    catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
    return true;
}

User db::getUser() {
    auto &db = db::getConnection();
    User user;

    try {
        SQLite::Statement select(db, "SELECT * FROM user LIMIT 1");
        if (select.executeStep()) {
            user.username = select.getColumn(0).getString();
            user.creationTime = select.getColumn(1).getInt();
            return user;
        }

    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
    }
    return user;
}

bool db::updateUsername(const std::string &oldUsername,
                        const std::string &newUsername) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement update(
            db, "UPDATE user SET username = ? WHERE username = ?");

        update.bind(1, newUsername);
        update.bind(2, oldUsername);
        update.exec();

    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return false;
    }
    return true;
}

bool db::createTask(const std::string &title, int priority, int status,
                    std::time_t dueDate) {
    auto &db = db::getConnection();

    try {
        SQLite::Statement insert(db,
                                 "INSERT INTO tasks (title, priority, status, "
                                 "dueDate, creationTime) VALUES (?, ?, "
                                 "?, ?, unixepoch())");

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
            return taskFromRow(select);
        }
        return std::nullopt;
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

std::vector<Task> db::getTasksByUser() {
    auto &db = db::getConnection();
    std::vector<Task> tasks;

    try {
        SQLite::Statement select(db, "SELECT * FROM tasks");

        while (select.executeStep()) {
            tasks.emplace_back(taskFromRow(select));
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
    }
    return tasks;
}

std::vector<Task> db::getIncompleteTasksByUser() {
    auto &db = db::getConnection();
    std::vector<Task> tasks;

    try {
        SQLite::Statement select(db,
                                 "SELECT * FROM tasks WHERE status IN (0, 1)");

        while (select.executeStep()) {
            tasks.emplace_back(taskFromRow(select));
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
