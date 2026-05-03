# Best Practices

```cpp
#include <libpq-fe.h>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <chrono>

// Best Practice 1: RAII wrapper for connection management
class DatabaseConnection {
public:
    DatabaseConnection(const std::string& conninfo) {
        conn_ = PQconnectdb(conninfo.c_str());
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::string error = PQerrorMessage(conn_);
            PQfinish(conn_);
            throw std::runtime_error("Connection failed: " + error);
        }
    }
    
    ~DatabaseConnection() {
        if (conn_) {
            PQfinish(conn_);
            conn_ = nullptr;
        }
    }
    
    // Prevent copying
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
    // Allow moving
    DatabaseConnection(DatabaseConnection&& other) noexcept : conn_(other.conn_) {
        other.conn_ = nullptr;
    }
    
    PGconn* get() { return conn_; }
    
    // Execute query with automatic error checking
    PGresult* execute(const std::string& query) {
        PGresult* res = PQexec(conn_, query.c_str());
        if (!res || (PQresultStatus(res) != PGRES_TUPLES_OK && 
                     PQresultStatus(res) != PGRES_COMMAND_OK)) {
            std::string error = res ? PQresultErrorMessage(res) : "Unknown error";
            if (res) PQclear(res);
            throw std::runtime_error("Query failed: " + error);
        }
        return res;
    }
    
private:
    PGconn* conn_ = nullptr;
};

// Best Practice 2: Connection pooling pattern
class ConnectionPool {
public:
    ConnectionPool(const std::string& conninfo, size_t pool_size) 
        : conninfo_(conninfo), pool_size_(pool_size) {
        for (size_t i = 0; i < pool_size; ++i) {
            pool_.push_back(createConnection());
        }
    }
    
    std::unique_ptr<DatabaseConnection> acquire() {
        if (pool_.empty()) {
            return std::make_unique<DatabaseConnection>(conninfo_);
        }
        auto conn = std::move(pool_.back());
        pool_.pop_back();
        return conn;
    }
    
    void release(std::unique_ptr<DatabaseConnection> conn) {
        if (pool_.size() < pool_size_) {
            pool_.push_back(std::move(conn));
        }
    }
    
private:
    std::unique_ptr<DatabaseConnection> createConnection() {
        return std::make_unique<DatabaseConnection>(conninfo_);
    }
    
    std::string conninfo_;
    size_t pool_size_;
    std::vector<std::unique_ptr<DatabaseConnection>> pool_;
};

// Best Practice 3: Prepared statement caching
class PreparedStatementCache {
public:
    void prepare(PGconn* conn, const std::string& name, const std::string& query) {
        PGresult* res = PQprepare(conn, name.c_str(), query.c_str(), 0, nullptr);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::string error = PQresultErrorMessage(res);
            PQclear(res);
            throw std::runtime_error("Prepare failed: " + error);
        }
        PQclear(res);
        statements_[name] = query;
    }
    
    PGresult* execute(PGconn* conn, const std::string& name, 
                      int nParams, const char* const* paramValues) {
        return PQexecPrepared(conn, name.c_str(), nParams, paramValues, 
                             nullptr, nullptr, 0);
    }
    
private:
    std::map<std::string, std::string> statements_;
};

// Best Practice 4: Transaction RAII
class Transaction {
public:
    Transaction(PGconn* conn) : conn_(conn) {
        PGresult* res = PQexec(conn_, "BEGIN");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            PQclear(res);
            throw std::runtime_error("Failed to begin transaction");
        }
        PQclear(res);
    }
    
    ~Transaction() {
        if (!committed_) {
            rollback();
        }
    }
    
    void commit() {
        PGresult* res = PQexec(conn_, "COMMIT");
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            PQclear(res);
            throw std::runtime_error("Failed to commit transaction");
        }
        PQclear(res);
        committed_ = true;
    }
    
    void rollback() {
        PGresult* res = PQexec(conn_, "ROLLBACK");
        if (res) PQclear(res);
        committed_ = true;  // Prevent double rollback
    }
    
private:
    PGconn* conn_;
    bool committed_ = false;
};

// Best Practice 5: Error handling with retry logic
template<typename Func>
auto with_retry(Func func, int max_retries = 3) -> decltype(func()) {
    int attempts = 0;
    while (true) {
        try {
            return func();
        } catch (const std::exception& e) {
            if (++attempts >= max_retries) {
                throw;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100 * attempts));
        }
    }
}

// Usage example
int main() {
    try {
        DatabaseConnection conn("host=localhost dbname=mydb user=myuser");
        
        // Use transaction RAII
        {
            Transaction txn(conn.get());
            
            // Execute queries within transaction
            PGresult* res = conn.execute("UPDATE accounts SET balance = balance - 100 WHERE id = 1");
            PQclear(res);
            
            res = conn.execute("UPDATE accounts SET balance = balance + 100 WHERE id = 2");
            PQclear(res);
            
            txn.commit();  // Auto-rollback if exception occurs
        }
        
        // Use prepared statements
        PreparedStatementCache cache;
        cache.prepare(conn.get(), "get_user", "SELECT * FROM users WHERE id = $1");
        
        const char* params[1] = { "42" };
        PGresult* res = cache.execute(conn.get(), "get_user", 1, params);
        PQclear(res);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```