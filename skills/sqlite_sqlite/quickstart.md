# Quickstart

```cpp
// Pattern 1: Open and close a database
#include <sqlite3.h>
#include <iostream>

int main() {
    sqlite3* db;
    int rc = sqlite3_open("example.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << "\n";
        return 1;
    }
    // Use database...
    sqlite3_close(db);
    return 0;
}
```

```cpp
// Pattern 2: Execute a simple query
#include <sqlite3.h>
#include <iostream>

int main() {
    sqlite3* db;
    sqlite3_open(":memory:", &db);
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, "CREATE TABLE test(id INT, name TEXT)", 
                          nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
    
    sqlite3_close(db);
    return 0;
}
```

```cpp
// Pattern 3: Prepared statements with parameters
#include <sqlite3.h>
#include <string>

int main() {
    sqlite3* db;
    sqlite3_open(":memory:", &db);
    sqlite3_exec(db, "CREATE TABLE users(id INT, name TEXT)", nullptr, nullptr, nullptr);
    
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "INSERT INTO users VALUES(?, ?)", -1, &stmt, nullptr);
    
    sqlite3_bind_int(stmt, 1, 42);
    sqlite3_bind_text(stmt, 2, "Alice", -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    sqlite3_close(db);
    return 0;
}
```

```cpp
// Pattern 4: Query with result iteration
#include <sqlite3.h>
#include <iostream>

int main() {
    sqlite3* db;
    sqlite3_open(":memory:", &db);
    sqlite3_exec(db, "CREATE TABLE items(val INT)", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "INSERT INTO items VALUES(10),(20),(30)", nullptr, nullptr, nullptr);
    
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT val FROM items", -1, &stmt, nullptr);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int val = sqlite3_column_int(stmt, 0);
        std::cout << "Value: " << val << "\n";
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
```

```cpp
// Pattern 5: Error handling with transactions
#include <sqlite3.h>
#include <iostream>

int main() {
    sqlite3* db;
    sqlite3_open(":memory:", &db);
    
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    
    char* errMsg = nullptr;
    if (sqlite3_exec(db, "UPDATE accounts SET balance = balance - 100 WHERE id = 1",
                     nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        std::cerr << "Error: " << errMsg << "\n";
        sqlite3_free(errMsg);
    } else {
        sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    }
    
    sqlite3_close(db);
    return 0;
}
```

```cpp
// Pattern 6: Using sqlite3_get_table for simple queries
#include <sqlite3.h>
#include <iostream>

int main() {
    sqlite3* db;
    sqlite3_open(":memory:", &db);
    sqlite3_exec(db, "CREATE TABLE t(a,b)", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "INSERT INTO t VALUES(1,'x'),(2,'y')", nullptr, nullptr, nullptr);
    
    char** result;
    int rows, cols;
    sqlite3_get_table(db, "SELECT * FROM t", &result, &rows, &cols, nullptr);
    
    for (int i = 0; i <= rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << result[i * cols + j] << "\t";
        }
        std::cout << "\n";
    }
    
    sqlite3_free_table(result);
    sqlite3_close(db);
    return 0;
}
```