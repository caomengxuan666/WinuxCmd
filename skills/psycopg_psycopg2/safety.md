# Safety

```cpp
#include <libpq-fe.h>
#include <iostream>
#include <cstring>

// RED LINE 1: NEVER use PQexec with untrusted input (SQL injection)
void never_sql_injection(PGconn* conn, const std::string& user_input) {
    // BAD: Direct string concatenation with user input
    std::string query = "SELECT * FROM users WHERE name = '" + user_input + "'";
    PGresult* res = PQexec(conn, query.c_str());  // DANGER: SQL injection
    PQclear(res);
}

void safe_parameterized(PGconn* conn, const std::string& user_input) {
    // GOOD: Always use parameterized queries for user input
    const char* params[1] = { user_input.c_str() };
    PGresult* res = PQexecParams(conn,
        "SELECT * FROM users WHERE name = $1",
        1, nullptr, params, nullptr, nullptr, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQresultErrorMessage(res) << std::endl;
    }
    PQclear(res);
}

// RED LINE 2: NEVER forget to clear PGresult objects
void never_memory_leak(PGconn* conn) {
    // BAD: Memory leak - result not cleared
    PGresult* res = PQexec(conn, "SELECT * FROM large_table");
    // res is never freed - memory leak
}

void always_clear_results(PGconn* conn) {
    // GOOD: Always clear results
    PGresult* res = PQexec(conn, "SELECT * FROM large_table");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed" << std::endl;
    }
    PQclear(res);  // Always free the result
}

// RED LINE 3: NEVER use a connection after PQfinish
void never_use_after_free() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb");
    PQfinish(conn);
    // BAD: Using connection after it's been destroyed
    PGresult* res = PQexec(conn, "SELECT 1");  // Undefined behavior - crash or corruption
}

void proper_connection_lifecycle() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb");
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed" << std::endl;
        PQfinish(conn);
        return;
    }
    // Use connection...
    PGresult* res = PQexec(conn, "SELECT 1");
    PQclear(res);
    PQfinish(conn);  // Last operation with this connection
}

// RED LINE 4: NEVER ignore PQstatus after connection attempt
void never_ignore_connection_status() {
    PGconn* conn = PQconnectdb("host=invalid dbname=nonexistent");
    // BAD: No status check before using connection
    PGresult* res = PQexec(conn, "SELECT 1");  // Will crash if connection failed
    PQclear(res);
    PQfinish(conn);
}

void always_check_connection(PGconn* conn) {
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection error: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return;
    }
    // Safe to use connection
}

// RED LINE 5: NEVER assume PQexec succeeded without checking result status
void never_ignore_result_status(PGconn* conn) {
    PGresult* res = PQexec(conn, "INVALID SQL");
    // BAD: Assume query succeeded
    int rows = PQntuples(res);  // May return -1 or crash
    std::cout << "Rows: " << rows << std::endl;
    PQclear(res);
}

void always_check_result(PGconn* conn) {
    PGresult* res = PQexec(conn, "INVALID SQL");
    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
        std::cerr << "Query failed: " << PQresultErrorMessage(res) << std::endl;
        PQclear(res);
        return;
    }
    // Safe to process results
    PQclear(res);
}
```