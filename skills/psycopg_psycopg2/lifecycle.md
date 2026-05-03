# Lifecycle

```cpp
#include <libpq-fe.h>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// Construction: Creating a database connection
void construction_examples() {
    // Method 1: Simple connection string
    PGconn* conn1 = PQconnectdb("host=localhost dbname=mydb user=myuser password=mypass");
    if (PQstatus(conn1) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn1) << std::endl;
        PQfinish(conn1);
        return;
    }
    
    // Method 2: Key-value parameters
    const char* keywords[] = {"host", "dbname", "user", "password", nullptr};
    const char* values[] = {"localhost", "mydb", "myuser", "mypass", nullptr};
    PGconn* conn2 = PQconnectdbParams(keywords, values, 0);
    if (PQstatus(conn2) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn2) << std::endl;
        PQfinish(conn2);
        return;
    }
    
    // Method 3: Asynchronous connection (non-blocking)
    PGconn* conn3 = PQconnectStart("host=localhost dbname=mydb");
    if (PQstatus(conn3) == CONNECTION_BAD) {
        PQfinish(conn3);
        return;
    }
    // Poll until connection is complete
    while (PQconnectPoll(conn3) != PGRES_POLLING_OK) {
        // Wait for socket events
    }
    
    PQfinish(conn1);
    PQfinish(conn2);
    PQfinish(conn3);
}

// Destruction: Proper cleanup
void destruction_examples() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb");
    if (PQstatus(conn) != CONNECTION_OK) {
        PQfinish(conn);
        return;
    }
    
    // Execute some queries
    PGresult* res1 = PQexec(conn, "SELECT 1");
    PGresult* res2 = PQexec(conn, "SELECT 2");
    
    // Cleanup order: results first, then connection
    PQclear(res1);
    PQclear(res2);
    PQfinish(conn);  // Last operation - connection is now invalid
}

// Move semantics: Transferring connection ownership
class ConnectionWrapper {
public:
    // Constructor
    ConnectionWrapper(const std::string& conninfo) {
        conn_ = PQconnectdb(conninfo.c_str());
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::string error = PQerrorMessage(conn_);
            PQfinish(conn_);
            throw std::runtime_error("Connection failed: " + error);
        }
    }
    
    // Destructor
    ~ConnectionWrapper() {
        if (conn_) {
            PQfinish(conn_);
            conn_ = nullptr;
        }
    }
    
    // Move constructor
    ConnectionWrapper(ConnectionWrapper&& other) noexcept 
        : conn_(other.conn_) {
        other.conn_ = nullptr;  // Transfer ownership
    }
    
    // Move assignment
    ConnectionWrapper& operator=(ConnectionWrapper&& other) noexcept {
        if (this != &other) {
            if (conn_) PQfinish(conn_);  // Release current
            conn_ = other.conn_;          // Take ownership
            other.conn_ = nullptr;        // Clear source
        }
        return *this;
    }
    
    // Delete copy operations
    ConnectionWrapper(const ConnectionWrapper&) = delete;
    ConnectionWrapper& operator=(const ConnectionWrapper&) = delete;
    
    PGconn* get() { return conn_; }
    
private:
    PGconn* conn_ = nullptr;
};

// Resource management: RAII for results
class ResultWrapper {
public:
    ResultWrapper(PGresult* res) : res_(res) {}
    
    ~ResultWrapper() {
        if (res_) {
            PQclear(res_);
            res_ = nullptr;
        }
    }
    
    // Move semantics
    ResultWrapper(ResultWrapper&& other) noexcept : res_(other.res_) {
        other.res_ = nullptr;
    }
    
    ResultWrapper& operator=(ResultWrapper&& other) noexcept {
        if (this != &other) {
            if (res_) PQclear(res_);
            res_ = other.res_;
            other.res_ = nullptr;
        }
        return *this;
    }
    
    // Delete copy
    ResultWrapper(const ResultWrapper&) = delete;
    ResultWrapper& operator=(const ResultWrapper&) = delete;
    
    PGresult* get() { return res_; }
    
private:
    PGresult* res_ = nullptr;
};

// Usage example demonstrating full lifecycle
int main() {
    try {
        // Construction
        ConnectionWrapper conn("host=localhost dbname=mydb user=myuser");
        
        // Move semantics
        ConnectionWrapper conn2 = std::move(conn);  // Transfer ownership
        // conn is now empty, conn2 owns the connection
        
        // Resource management
        ResultWrapper res(PQexec(conn2.get(), "SELECT * FROM users"));
        
        // Process results...
        int rows = PQntuples(res.get());
        std::cout << "Found " << rows << " users" << std::endl;
        
        // Automatic cleanup when wrappers go out of scope
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```