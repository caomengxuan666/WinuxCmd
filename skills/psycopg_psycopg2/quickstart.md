# Quickstart

```cpp
#include <iostream>
#include <string>
#include <libpq-fe.h>
#include <memory>
#include <stdexcept>

// Pattern 1: Connect to PostgreSQL database
void connect_example() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb user=myuser password=mypass");
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return;
    }
    std::cout << "Connected successfully" << std::endl;
    PQfinish(conn);
}

// Pattern 2: Execute a simple query
void query_example(PGconn* conn) {
    PGresult* res = PQexec(conn, "SELECT * FROM users LIMIT 5");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return;
    }
    int rows = PQntuples(res);
    int cols = PQnfields(res);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << PQgetvalue(res, i, j) << "\t";
        }
        std::cout << std::endl;
    }
    PQclear(res);
}

// Pattern 3: Parameterized query (safe from SQL injection)
void parameterized_query(PGconn* conn, const std::string& username) {
    const char* paramValues[1] = { username.c_str() };
    PGresult* res = PQexecParams(conn,
        "SELECT * FROM users WHERE username = $1",
        1, nullptr, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return;
    }
    // Process results...
    PQclear(res);
}

// Pattern 4: Transaction management
void transaction_example(PGconn* conn) {
    PGresult* res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        return;
    }
    PQclear(res);
    
    res = PQexec(conn, "UPDATE accounts SET balance = balance - 100 WHERE id = 1");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQexec(conn, "ROLLBACK");
        PQclear(res);
        return;
    }
    PQclear(res);
    
    res = PQexec(conn, "COMMIT");
    PQclear(res);
}

// Pattern 5: Prepared statements
void prepared_statement_example(PGconn* conn) {
    PGresult* res = PQprepare(conn, "get_user",
        "SELECT * FROM users WHERE id = $1", 1, nullptr);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Prepare failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return;
    }
    PQclear(res);
    
    const char* paramValues[1] = { "42" };
    res = PQexecPrepared(conn, "get_user", 1, paramValues, nullptr, nullptr, 0);
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        // Process results...
    }
    PQclear(res);
}

// Pattern 6: Error handling wrapper
class PGConnection {
public:
    PGConnection(const std::string& conninfo) {
        conn_ = PQconnectdb(conninfo.c_str());
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::string err = PQerrorMessage(conn_);
            PQfinish(conn_);
            throw std::runtime_error("Connection failed: " + err);
        }
    }
    ~PGConnection() { if (conn_) PQfinish(conn_); }
    PGconn* get() { return conn_; }
private:
    PGconn* conn_;
};

int main() {
    try {
        PGConnection conn("host=localhost dbname=mydb user=myuser");
        query_example(conn.get());
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```