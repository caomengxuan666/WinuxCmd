# Best Practices

```cpp
// Use RAII wrappers for automatic resource management
#include <sqlite3.h>
#include <memory>
#include <string>

class Database {
    sqlite3* db_;
public:
    Database(const std::string& path) {
        if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db_));
        }
    }
    
    ~Database() {
        if (db_) sqlite3_close(db_);
    }
    
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    Database(Database&& other) noexcept : db_(other.db_) {
        other.db_ = nullptr;
    }
    
    sqlite3* get() const { return db_; }
};
```

```cpp
// Use WAL mode for better concurrent read performance
#include <sqlite3.h>

void enableWAL(sqlite3* db) {
    char* errMsg = nullptr;
    sqlite3_exec(db, "PRAGMA journal_mode=WAL", nullptr, nullptr, &errMsg);
    if (errMsg) {
        sqlite3_free(errMsg);
    }
    sqlite3_exec(db, "PRAGMA synchronous=NORMAL", nullptr, nullptr, nullptr);
}
```

```cpp
// Implement a prepared statement cache
#include <unordered_map>
#include <string>

class StatementCache {
    sqlite3* db_;
    std::unordered_map<std::string, sqlite3_stmt*> cache_;
    
public:
    sqlite3_stmt* get(const std::string& sql) {
        auto it = cache_.find(sql);
        if (it != cache_.end()) {
            sqlite3_reset(it->second);
            return it->second;
        }
        
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        cache_[sql] = stmt;
        return stmt;
    }
    
    ~StatementCache() {
        for (auto& [_, stmt] : cache_) {
            sqlite3_finalize(stmt);
        }
    }
};
```

```cpp
// Use explicit transactions for atomic operations
bool transferFunds(sqlite3* db, int from, int to, double amount) {
    char* errMsg = nullptr;
    
    sqlite3_exec(db, "BEGIN IMMEDIATE", nullptr, nullptr, &errMsg);
    
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "UPDATE accounts SET balance = balance - ? WHERE id = ?",
                       -1, &stmt, nullptr);
    sqlite3_bind_double(stmt, 1, amount);
    sqlite3_bind_int(stmt, 2, from);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    
    sqlite3_prepare_v2(db, "UPDATE accounts SET balance = balance + ? WHERE id = ?",
                       -1, &stmt, nullptr);
    sqlite3_bind_double(stmt, 1, amount);
    sqlite3_bind_int(stmt, 2, to);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    return true;
}
```