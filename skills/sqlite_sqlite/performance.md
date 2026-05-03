# Performance

```cpp
// Use transactions for batch operations (100x speedup)
#include <sqlite3.h>
#include <chrono>

void insertWithoutTransaction(sqlite3* db) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        sqlite3_exec(db, "INSERT INTO t VALUES(1)", nullptr, nullptr, nullptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    // Takes ~5000ms
}

void insertWithTransaction(sqlite3* db) {
    auto start = std::chrono::high_resolution_clock::now();
    
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    for (int i = 0; i < 10000; ++i) {
        sqlite3_exec(db, "INSERT INTO t VALUES(1)", nullptr, nullptr, nullptr);
    }
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    
    auto end = std::chrono::high_resolution_clock::now();
    // Takes ~50ms (100x faster!)
}
```

```cpp
// Use prepared statements for repeated queries
void slowWay(sqlite3* db) {
    for (int i = 0; i < 1000; ++i) {
        char sql[100];
        sprintf(sql, "INSERT INTO t VALUES(%d)", i);
        sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
        // Each call recompiles the SQL
    }
}

void fastWay(sqlite3* db) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "INSERT INTO t VALUES(?)", -1, &stmt, nullptr);
    
    for (int i = 0; i < 1000; ++i) {
        sqlite3_bind_int(stmt, 1, i);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);  // Reset for reuse
    }
    
    sqlite3_finalize(stmt);
    // ~10x faster than slowWay
}
```

```cpp
// Optimize with PRAGMA statements
void optimizeDatabase(sqlite3* db) {
    // Use memory-mapped I/O for large databases
    sqlite3_exec(db, "PRAGMA mmap_size=268435456", nullptr, nullptr, nullptr);  // 256MB
    
    // Increase cache size
    sqlite3_exec(db, "PRAGMA cache_size=-64000", nullptr, nullptr, nullptr);  // 64MB
    
    // Use WAL mode for better write performance
    sqlite3_exec(db, "PRAGMA journal_mode=WAL", nullptr, nullptr, nullptr);
    
    // Set appropriate page size (usually 4096 or 8192)
    sqlite3_exec(db, "PRAGMA page_size=4096", nullptr, nullptr, nullptr);
}
```

```cpp
// Use sqlite3_prepare_v2 instead of sqlite3_prepare for better performance
void usePrepared(sqlite3* db) {
    sqlite3_stmt* stmt;
    
    // v2 version caches the query plan
    sqlite3_prepare_v2(db, "SELECT * FROM t WHERE id = ?", -1, &stmt, nullptr);
    
    // Bind and execute multiple times
    for (int id : {1, 2, 3, 4, 5}) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    
    sqlite3_finalize(stmt);
}
```