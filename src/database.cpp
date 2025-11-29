#include "database.h"

#include <exception>
#include <optional>
#include <print>
#include <string>

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
        "dueDate INTEGER, ownerId INTEGER NOT NULL, creationTime INTEGER NOT "
        "NULL, FOREIGN KEY (ownerId) REFERENCES users(id) ON DELETE CASCADE);");

    db.exec(
        "CREATE TABLE IF NOT EXISTS teams (id INTEGER NOT NULL PRIMARY KEY "
        "AUTOINCREMENT, "
        "name TEXT NOT NULL, creatorId INTEGER NOT NULL, FOREIGN KEY "
        "(creatorId) REFERENCES users(id) ON DELETE CASCADE);");
}

SQLite::Database& db::getConnection() {
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

bool db::createUser(const std::string& username, const std::string& email,

                    const std::string& password) {
    auto& db = db::getConnection();

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

    catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return false;
    }
    return true;
}

std::optional<User> db::getUser(const std::string& username) {
    auto& db = db::getConnection();

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
    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

std::optional<User> db::getUserById(int id) {
    auto& db = db::getConnection();

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
    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

std::optional<User> db::getUserByEmail(const std::string& email) {
    auto& db = db::getConnection();

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
    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return std::nullopt;
    }
}

bool db::deleteUser(int id) {
    auto& db = db::getConnection();

    try {
        SQLite::Statement del(db, "DELETE FROM users WHERE id = ?");
        del.bind(1, id);
        del.exec();

    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return false;
    }

    return true;
}

// TODO: Refactor such that getUser when not logged in calls authenticateUser
// before returning.
bool db::authenticateUser(const std::string& email,
                          const std::string& password) {
    auto& db = db::getConnection();

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

    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updateEmail(int userId, const std::string& newEmail) {
    auto& db = db::getConnection();

    try {
        SQLite::Statement update(db, "UPDATE users SET email = ? WHERE id = ?");
        update.bind(1, newEmail);
        update.bind(2, userId);
        update.exec();

        return true;
    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updatePassword(int userId, const std::string& newPassword) {
    auto& db = db::getConnection();

    try {
        SQLite::Statement update(
            db, "UPDATE users SET passwordHash = ? WHERE id = ?");
        update.bind(1, security::hashPassword(newPassword));
        update.bind(2, userId);
        update.exec();

        return true;
    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::updateUsername(int userId, const std::string& newUsername) {
    auto& db = db::getConnection();

    try {
        SQLite::Statement update(db,
                                 "UPDATE users SET username = ? WHERE id = ?");
        update.bind(1, newUsername);
        update.bind(2, userId);
        update.exec();

        return true;
    } catch (const std::exception& e) {
        std::println("{}\n", e.what());
        return false;
    }
}

bool db::createTask(const std::string& title, int priority, std::time_t dueDate,
                    int ownerId);
std::optional<Task> db::getTask(int id);
std::vector<Task> db::getTasksByUser(int userId);
bool db::updateTask(int id /* fields to update */);
bool db::deleteTask(int id);
