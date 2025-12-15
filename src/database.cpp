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

    db.exec(
        "CREATE TABLE IF NOT EXISTS task_dependencies ("
        "taskId INTEGER NOT NULL, "
        "dependsOnTaskId INTEGER NOT NULL, "
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
            dependencies.push_back(taskFromRow(select));
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
            dependents.push_back(taskFromRow(select));
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
    }
    return dependents;
}

bool db::wouldCreateCycle(int taskId, int dependsOnTaskId) {
    auto &db = db::getConnection();

    try {
        // Check if dependsOnTaskId can reach taskId through dependencies
        // Using recursive CTE to traverse the dependency chain
        SQLite::Statement query(
            db,
            "WITH RECURSIVE dep_chain(id) AS ("
            "  SELECT ? "
            "  UNION ALL "
            "  SELECT td.dependsOnTaskId "
            "  FROM task_dependencies td "
            "  JOIN dep_chain dc ON td.taskId = dc.id"
            ") "
            "SELECT 1 FROM dep_chain WHERE id = ? LIMIT 1");

        query.bind(1, dependsOnTaskId);
        query.bind(2, taskId);

        return query.executeStep();
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return true;  // Assume cycle on error to be safe
    }
}

std::vector<int> db::getTopologicalOrder() {
    auto &db = db::getConnection();
    std::vector<int> order;

    try {
        // Get all task IDs first
        std::vector<int> allTasks;
        SQLite::Statement getAllTasks(db, "SELECT id FROM tasks");
        while (getAllTasks.executeStep()) {
            allTasks.push_back(getAllTasks.getColumn(0).getInt());
        }

        std::vector<int> processed;

        // Iteratively find tasks with no unprocessed dependencies
        while (processed.size() < allTasks.size()) {
            bool foundTask = false;

            for (int taskId : allTasks) {
                // Skip if already processed
                bool alreadyProcessed = false;
                for (int p : processed) {
                    if (p == taskId) {
                        alreadyProcessed = true;
                        break;
                    }
                }
                if (alreadyProcessed) continue;

                // Check if all dependencies are processed
                bool allDepsProcessed = true;
                SQLite::Statement getDeps(
                    db,
                    "SELECT dependsOnTaskId FROM task_dependencies WHERE "
                    "taskId = ?");
                getDeps.bind(1, taskId);

                while (getDeps.executeStep()) {
                    int depId = getDeps.getColumn(0).getInt();
                    bool depProcessed = false;
                    for (int p : processed) {
                        if (p == depId) {
                            depProcessed = true;
                            break;
                        }
                    }
                    if (!depProcessed) {
                        allDepsProcessed = false;
                        break;
                    }
                }

                if (allDepsProcessed) {
                    order.push_back(taskId);
                    processed.push_back(taskId);
                    foundTask = true;
                    break;
                }
            }

            // If no task found, there's a cycle
            if (!foundTask && processed.size() < allTasks.size()) {
                std::println("Cycle detected - cannot create topological order");
                return {};
            }
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return {};
    }

    return order;
}

std::vector<int> db::getCriticalPath() {
    auto &db = db::getConnection();
    std::vector<int> path;

    try {
        // Find tasks with no dependents (leaf tasks)
        std::vector<int> leafTasks;
        SQLite::Statement findLeaves(
            db,
            "SELECT id FROM tasks WHERE id NOT IN "
            "(SELECT DISTINCT dependsOnTaskId FROM task_dependencies)");

        while (findLeaves.executeStep()) {
            leafTasks.push_back(findLeaves.getColumn(0).getInt());
        }

        if (leafTasks.empty()) {
            return path;
        }

        // For each leaf, calculate depth recursively
        int maxDepth = 0;
        int criticalLeaf = -1;

        for (int leafId : leafTasks) {
            int depth = 0;
            std::vector<int> currentPath;
            std::vector<int> visited;

            // Calculate depth by following dependencies backward
            int currentId = leafId;
            currentPath.push_back(currentId);

            while (true) {
                // Find the dependency with maximum depth
                SQLite::Statement getDeps(
                    db,
                    "SELECT dependsOnTaskId FROM task_dependencies WHERE "
                    "taskId = ?");
                getDeps.bind(1, currentId);

                int maxDepthDep = -1;
                int nextId = -1;

                while (getDeps.executeStep()) {
                    int depId = getDeps.getColumn(0).getInt();

                    // Avoid cycles in visited
                    bool alreadyVisited = false;
                    for (int v : visited) {
                        if (v == depId) {
                            alreadyVisited = true;
                            break;
                        }
                    }
                    if (!alreadyVisited) {
                        // For simplicity, just pick first unvisited dependency
                        nextId = depId;
                        break;
                    }
                }

                if (nextId == -1) {
                    // No more dependencies
                    break;
                }

                visited.push_back(currentId);
                currentId = nextId;
                currentPath.push_back(currentId);
                depth++;
            }

            if (depth > maxDepth) {
                maxDepth = depth;
                criticalLeaf = leafId;
                path = currentPath;
            }
        }
    } catch (const std::exception &e) {
        std::println("{}\n", e.what());
        return {};
    }

    return path;
}
