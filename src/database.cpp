#pragma once
#include "database.h"

#include "SQLiteCpp/Database.h"

void initStorage() {
    SQLite::Database db(DATABASE_FILE,
                        SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

    db.exec("PRAGMA foreign_keys = ON;");

    db.exec(
        "CREATE TABLE users (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, email TEXT NOT NULL, passwordHash TEXT NOT "
        "NULL, creationTime INTEGER NOT NULL);");

    db.exec(
        "CREATE TABLE tasks (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, priority INTEGER, status INTEGER NOT NULL, "
        "dueDate INTEGER, ownerId INTEGER NOT NULL, creationTime INTEGER NOT "
        "NULL, FOREIGN KEY (ownerId) REFERENCES users(id) ON DELETE CASCADE);");

    db.exec(
        "CREATE TABLE teams (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, creatorId INTEGER NOT NULL, FOREIGN KEY "
        "(creatorId) REFERENCES users(id) ON DELETE CASCADE);");
}
