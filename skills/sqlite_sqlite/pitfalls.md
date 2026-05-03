# Pitfalls

```cpp
// BAD: Not checking return codes
sqlite3* db;
sqlite3_open("data.db", &db);  // Might fail silently
sqlite3_exec(db, "SELECT * FROM t", nullptr, nullptr, nullptr);  // Crash if db is null

// GOOD: Always check return codes
sqlite3* db;
int rc = sqlite3_open("data.db", &db);
if (rc != SQLITE_OK) {
    std::cerr << "Failed to open: " << sqlite3_errmsg(db) << "\n";
    sqlite3_close(db);
    return;
}
```

```cpp
// BAD: Forgetting to finalize prepared statements (memory leak)
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT 1", -1, &stmt, nullptr);
sqlite3_step(stmt);
// stmt never finalized - memory leak!

// GOOD: Always finalize prepared statements
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT 1", -1, &stmt, nullptr);
sqlite3_step(stmt);
sqlite3_finalize(stmt);  // Required to free resources
```

```cpp
// BAD: Using string concatenation for SQL (SQL injection risk)
std::string name = getUserInput();  // Could be "'; DROP TABLE users; --"
std::string sql = "SELECT * FROM users WHERE name = '" + name + "'";
sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

// GOOD: Use parameterized queries
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT * FROM users WHERE name = ?", -1, &stmt, nullptr);
sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
sqlite3_step(stmt);
sqlite3_finalize(stmt);
```

```cpp
// BAD: Not handling SQLITE_BUSY errors
sqlite3_exec(db, "BEGIN EXCLUSIVE", nullptr, nullptr, nullptr);
// Another connection might have a lock
sqlite3_exec(db, "UPDATE t SET x = 1", nullptr, nullptr, nullptr);  // Might fail with SQLITE_BUSY

// GOOD: Implement retry logic for busy conditions
int rc;
do {
    rc = sqlite3_exec(db, "UPDATE t SET x = 1", nullptr, nullptr, nullptr);
    if (rc == SQLITE_BUSY) {
        usleep(1000);  // Wait 1ms before retry
    }
} while (rc == SQLITE_BUSY);
```

```cpp
// BAD: Using sqlite3_close on a busy database
sqlite3* db;
sqlite3_open(":memory:", &db);
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT 1", -1, &stmt, nullptr);
sqlite3_close(db);  // Returns SQLITE_BUSY, database not closed!
// stmt still exists, but db is invalid

// GOOD: Finalize all statements before closing
sqlite3_finalize(stmt);
sqlite3_close(db);  // Now safe
```

```cpp
// BAD: Ignoring column type mismatches
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT 'hello'", -1, &stmt, nullptr);
sqlite3_step(stmt);
int val = sqlite3_column_int(stmt, 0);  // Returns 0, but no error!

// GOOD: Check column type before accessing
int type = sqlite3_column_type(stmt, 0);
if (type == SQLITE_INTEGER) {
    int val = sqlite3_column_int(stmt, 0);
} else if (type == SQLITE_TEXT) {
    const char* text = sqlite3_column_text(stmt, 0);
}
```

```cpp
// BAD: Not using transactions for bulk inserts
for (int i = 0; i < 10000; ++i) {
    sqlite3_exec(db, "INSERT INTO t VALUES(1)", nullptr, nullptr, nullptr);
    // Each insert is its own transaction - very slow!
}

// GOOD: Wrap bulk operations in a transaction
sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
for (int i = 0; i < 10000; ++i) {
    sqlite3_exec(db, "INSERT INTO t VALUES(1)", nullptr, nullptr, nullptr);
}
sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
```