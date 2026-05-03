# Lifecycle

```cpp
// Construction: Opening a database
#include <sqlite3.h>

sqlite3* db;
int rc = sqlite3_open("mydb.sqlite", &db);
// rc == SQLITE_OK means success
// rc == SQLITE_CANTOPEN means file cannot be opened
// rc == SQLITE_NOMEM means out of memory

// For in-memory databases:
sqlite3_open(":memory:", &db);

// For temporary files:
sqlite3_open("", &db);
```

```cpp
// Destruction: Proper cleanup sequence
void cleanupDatabase(sqlite3* db) {
    // 1. Finalize all prepared statements
    // (This should be done individually for each statement)
    
    // 2. Close the database
    int rc = sqlite3_close(db);
    if (rc == SQLITE_BUSY) {
        // There are unfinalized statements - find and finalize them
        // sqlite3_close_v2() can be used as an alternative
        sqlite3_close_v2(db);  // Defers close until all statements are finalized
    }
}
```

```cpp
// Move semantics: Transferring database ownership
#include <utility>

class DatabaseOwner {
    sqlite3* db_;
public:
    DatabaseOwner(const char* path) {
        sqlite3_open(path, &db_);
    }
    
    ~DatabaseOwner() {
        if (db_) sqlite3_close(db_);
    }
    
    // Move constructor
    DatabaseOwner(DatabaseOwner&& other) noexcept 
        : db_(std::exchange(other.db_, nullptr)) {}
    
    // Move assignment
    DatabaseOwner& operator=(DatabaseOwner&& other) noexcept {
        if (this != &other) {
            if (db_) sqlite3_close(db_);
            db_ = std::exchange(other.db_, nullptr);
        }
        return *this;
    }
    
    // No copy semantics
    DatabaseOwner(const DatabaseOwner&) = delete;
    DatabaseOwner& operator=(const DatabaseOwner&) = delete;
};
```

```cpp
// Resource management with backup API
void backupDatabase(sqlite3* source, const char* destPath) {
    sqlite3* dest;
    sqlite3_open(destPath, &dest);
    
    sqlite3_backup* backup = sqlite3_backup_init(dest, "main", source, "main");
    if (backup) {
        sqlite3_backup_step(backup, -1);  // Copy entire database
        sqlite3_backup_finish(backup);
    }
    
    sqlite3_close(dest);
}
```