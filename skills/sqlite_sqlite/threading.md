# Threading

```cpp
// SQLite thread safety modes
#include <sqlite3.h>

// Default: Single-thread mode (no mutexes)
// sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);

// Multi-thread mode (safe if no single connection used from multiple threads)
// sqlite3_config(SQLITE_CONFIG_MULTITHREAD);

// Serialized mode (full thread safety, default)
// sqlite3_config(SQLITE_CONFIG_SERIALIZED);

// Check current mode
int main() {
    if (sqlite3_threadsafe()) {
        std::cout << "SQLite is compiled with thread safety\n";
    }
    
    int mode;
    sqlite3_config(SQLITE_CONFIG_GETMODE, &mode);
    // mode == 0: single-thread
    // mode == 1: multi-thread  
    // mode == 2: serialized
}
```

```cpp
// Thread-safe pattern: One connection per thread
#include <thread>
#include <vector>

void threadWorker(int id) {
    sqlite3* db;
    sqlite3_open("shared.db", &db);
    
    // Each thread has its own connection
    sqlite3_exec(db, "BEGIN IMMEDIATE", nullptr, nullptr, nullptr);
    // ... do work ...
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    
    sqlite3_close(db);
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(threadWorker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
```

```cpp
// Using mutex to protect shared database access
#include <mutex>

class ThreadSafeDatabase {
    sqlite3* db_;
    std::mutex mutex_;
    
public:
    void execute(const std::string& sql) {
        std::lock_guard<std::mutex> lock(mutex_);
        sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, nullptr);
    }
    
    // For transactions, lock the entire transaction
    void executeTransaction(const std::string& sql1, const std::string& sql2) {
        std::lock_guard<std::mutex> lock(mutex_);
        sqlite3_exec(db_, "BEGIN", nullptr, nullptr, nullptr);
        sqlite3_exec(db_, sql1.c_str(), nullptr, nullptr, nullptr);
        sqlite3_exec(db_, sql2.c_str(), nullptr, nullptr, nullptr);
        sqlite3_exec(db_, "COMMIT", nullptr, nullptr, nullptr);
    }
};
```

```cpp
// WAL mode for concurrent reads
void setupWALForConcurrency(sqlite3* db) {
    // WAL mode allows one writer and multiple concurrent readers
    sqlite3_exec(db, "PRAGMA journal_mode=WAL", nullptr, nullptr, nullptr);
    
    // Set busy timeout instead of immediately failing
    sqlite3_busy_timeout(db, 5000);  // 5 second timeout
    
    // Configure WAL checkpointing
    sqlite3_exec(db, "PRAGMA wal_autocheckpoint=1000", nullptr, nullptr, nullptr);
}

// Reader threads can read while writer is active
void readerThread(const char* path) {
    sqlite3* db;
    sqlite3_open(path, &db);
    
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT * FROM t", -1, &stmt, nullptr);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Read data - this works even if another thread is writing
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
```