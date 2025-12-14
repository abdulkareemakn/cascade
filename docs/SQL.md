# SQLite/SQLiteCpp Pitfalls and Best Practices

## 1. Foreign Keys Not Enabled by Default
**CRITICAL**: `PRAGMA foreign_keys = ON;` must be run **every time** you open a connection. It's not persistent.

```cpp
SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
db.exec("PRAGMA foreign_keys = ON;");  // ALWAYS do this
```

## 2. UNIQUE Constraints Missing
Add UNIQUE constraints where needed:
```sql
CREATE TABLE users (
    username TEXT UNIQUE NOT NULL,
    email TEXT NOT NULL
);
```
Otherwise you can insert duplicate usernames.

## 3. Transactions for Multiple Operations
SQLiteCpp doesn't auto-wrap in transactions. For multiple inserts/updates:
```cpp
SQLite::Transaction transaction(db);
// ... multiple operations
transaction.commit();
```
Without this, each operation auto-commits (slow for bulk operations).

## 4. Statement Binding is 1-indexed
```cpp
statement.bind(1, value);  // First parameter is 1, not 0
statement.bind(2, value2); // Second parameter is 2
```

## 5. executeStep() Returns bool
```cpp
while (query.executeStep()) {  // true while rows available
    // process row
}
```
Don't use `exec()` for SELECT queries - use `executeStep()`.

## 6. Column Access Can Throw
```cpp
query.getColumn(0).getInt();  // Can throw if NULL or wrong type
```
Use `isNull()` checks for nullable columns:
```cpp
if (!query.getColumn(0).isNull()) {
    int value = query.getColumn(0).getInt();
}
```

## 7. Database File Locking
SQLite locks the entire database file. Concurrent writes will fail. Design for single-writer or use WAL mode:
```cpp
db.exec("PRAGMA journal_mode = WAL;");
```

## 8. TEXT vs BLOB for Strings
Use `TEXT` for strings, not `BLOB`. SQLiteCpp handles `std::string` → TEXT automatically.

## 9. No CASCADE on UNIQUE
If you add `UNIQUE` constraints, they don't cascade delete. Only foreign keys do.

## 10. ALTER TABLE Limitations
SQLite has very limited `ALTER TABLE` support. You often need to recreate tables for schema changes. Plan your schema carefully upfront.

Common pattern for schema changes:
```sql
-- 1. Create new table with desired schema
CREATE TABLE users_new (...);
-- 2. Copy data
INSERT INTO users_new SELECT ... FROM users;
-- 3. Drop old table
DROP TABLE users;
-- 4. Rename new table
ALTER TABLE users_new RENAME TO users;
```

## 11. IF NOT EXISTS
Always use `IF NOT EXISTS` for CREATE TABLE to make initialization idempotent:
```sql
CREATE TABLE IF NOT EXISTS users (...);
```

## 12. Unix Timestamps
Store as INTEGER, use `unixepoch()` or `strftime('%s', 'now')`:
```sql
INSERT INTO users (creation_time) VALUES (unixepoch());
```
