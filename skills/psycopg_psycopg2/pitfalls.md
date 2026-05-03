# Pitfalls

```cpp
#include <libpq-fe.h>
#include <iostream>
#include <string>

// PITFALL 1: Not checking PQstatus after connection
void bad_connect() {
    PGconn* conn = PQconnectdb("host=invalid dbname=nonexistent");
    // BAD: No status check before using connection
    PGresult* res = PQexec(conn, "SELECT 1");  // Crash if connection failed
    PQclear(res);
    PQfinish(conn);
}

void good_connect() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb");
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return;
    }
    // Safe to use connection
    PGresult* res = PQexec(conn, "SELECT 1");
    PQclear(res);
    PQfinish(conn);
}

// PITFALL 2: Forgetting to clear PGresult objects
void bad_memory_leak(PGconn* conn) {
    for (int i = 0; i < 1000; ++i) {
        PQexec(conn, "SELECT * FROM large_table");  // BAD: Memory leak each iteration
    }
}

void good_memory_management(PGconn* conn) {
    for (int i = 0; i < 1000; ++i) {
        PGresult* res = PQexec(conn, "SELECT * FROM large_table");
        if (res) PQclear(res);  // GOOD: Always clear results
    }
}

// PITFALL 3: SQL injection via string concatenation
void bad_sql_injection(PGconn* conn, const std::string& user_input) {
    std::string query = "SELECT * FROM users WHERE name = '" + user_input + "'";
    // BAD: Direct string concatenation allows SQL injection
    PGresult* res = PQexec(conn, query.c_str());
    PQclear(res);
}

void good_parameterized(PGconn* conn, const std::string& user_input) {
    const char* params[1] = { user_input.c_str() };
    // GOOD: Use parameterized queries
    PGresult* res = PQexecParams(conn,
        "SELECT * FROM users WHERE name = $1",
        1, nullptr, params, nullptr, nullptr, 0);
    PQclear(res);
}

// PITFALL 4: Not checking result status
void bad_result_check(PGconn* conn) {
    PGresult* res = PQexec(conn, "INVALID SQL");
    // BAD: Assume query succeeded
    int rows = PQntuples(res);  // May return -1 or crash
    std::cout << "Rows: " << rows << std::endl;
    PQclear(res);
}

void good_result_check(PGconn* conn) {
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

// PITFALL 5: Using connection after PQfinish
void bad_use_after_free() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb");
    PQfinish(conn);
    // BAD: Using connection after it's been destroyed
    PGresult* res = PQexec(conn, "SELECT 1");  // Undefined behavior
}

void good_lifecycle() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb");
    if (PQstatus(conn) != CONNECTION_OK) {
        PQfinish(conn);
        return;
    }
    // Use connection...
    PGresult* res = PQexec(conn, "SELECT 1");
    PQclear(res);
    PQfinish(conn);
    // conn is now invalid, don't use it
}

// PITFALL 6: Ignoring PQerrorMessage for debugging
void bad_no_error_info(PGconn* conn) {
    if (PQstatus(conn) != CONNECTION_OK) {
        // BAD: No error details
        return;
    }
}

void good_error_info(PGconn* conn) {
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection error: " << PQerrorMessage(conn) << std::endl;
        return;
    }
}
```