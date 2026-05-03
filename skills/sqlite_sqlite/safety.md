# Safety

**Condition 1: NEVER use sqlite3_close() while prepared statements are still open**
```cpp
// BAD: Closing database with active statements
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT 1", -1, &stmt, nullptr);
sqlite3_close(db);  // Returns SQLITE_BUSY, db not closed, resource leak

// GOOD: Finalize all statements first
sqlite3_finalize(stmt);
sqlite3_close(db);  // Safe
```

**Condition 2: NEVER pass user input directly into SQL without parameterization**
```cpp
// BAD: SQL injection vulnerability
std::string userInput = "'; DROP TABLE users; --";
std::string query = "SELECT * FROM users WHERE name = '" + userInput + "'";
sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);

// GOOD: Use bound parameters
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT * FROM users WHERE name = ?", -1, &stmt, nullptr);
sqlite3_bind_text(stmt, 1, userInput.c_str(), -1, SQLITE_TRANSIENT);
```

**Condition 3: NEVER ignore the return value of sqlite3_open()**
```cpp
// BAD: Assuming open always succeeds
sqlite3* db;
sqlite3_open("/nonexistent/path/db.sqlite", &db);  // Might fail
sqlite3_exec(db, "CREATE TABLE t(id INT)", nullptr, nullptr, nullptr);  // Crash if db is null

// GOOD: Check return code
sqlite3* db;
int rc = sqlite3_open("/nonexistent/path/db.sqlite", &db);
if (rc != SQLITE_OK) {
    // Handle error
    return;
}
```

**Condition 4: NEVER use sqlite3_column_*() without checking sqlite3_step() returned SQLITE_ROW**
```cpp
// BAD: Accessing column data without checking step result
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT 1", -1, &stmt, nullptr);
sqlite3_step(stmt);  // Might return SQLITE_DONE if no rows
int val = sqlite3_column_int(stmt, 0);  // Undefined behavior if no row

// GOOD: Check step result
if (sqlite3_step(stmt) == SQLITE_ROW) {
    int val = sqlite3_column_int(stmt, 0);
}
```

**Condition 5: NEVER modify a database while iterating over it with the same connection**
```cpp
// BAD: Modifying table while iterating
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT id FROM t", -1, &stmt, nullptr);
while (sqlite3_step(stmt) == SQLITE_ROW) {
    sqlite3_exec(db, "DELETE FROM t WHERE id = 1", nullptr, nullptr, nullptr);  // Undefined behavior!
}

// GOOD: Collect IDs first, then modify
std::vector<int> ids;
while (sqlite3_step(stmt) == SQLITE_ROW) {
    ids.push_back(sqlite3_column_int(stmt, 0));
}
sqlite3_finalize(stmt);
for (int id : ids) {
    // Now safe to modify
}
```