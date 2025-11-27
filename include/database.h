#pragma once

#include "models.h"
#include "sqlite_orm.h"

void initStorage();

inline auto& getStorage() {
    static auto storage = sqlite_orm::make_storage(
        DATABASE_FILE,
        sqlite_orm::make_table(
            "users",
            sqlite_orm::make_column("id", &User::id,
                                    sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("username", &User::username),
            sqlite_orm::make_column("email", &User::email),
            sqlite_orm::make_column("passwordHash", &User::passwordHash),
            sqlite_orm::make_column(
                "creationTime", &User::creationTime,
                sqlite_orm::default_value(sqlite_orm::current_time()))),

        sqlite_orm::make_table(
            "tasks",
            sqlite_orm::make_column("id", &Task::id,
                                    sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("title", &Task::title),
            sqlite_orm::make_column("priority", &Task::priority),
            sqlite_orm::make_column("creationTime", &Task::creationTime),
            sqlite_orm::make_column("dueDate", &Task::dueDate),
            sqlite_orm::make_column("ownerId", &Task::ownerId),
            sqlite_orm::foreign_key(&Task::ownerId).references(&User::id)));

    return storage;
}
