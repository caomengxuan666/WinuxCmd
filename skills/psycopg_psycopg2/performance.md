# Performance

```cpp
#include <libpq-fe.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

// Performance Characteristic 1: Batch operations vs individual queries
void bad_individual_queries(PGconn* conn) {
    // BAD: N+1 query problem - executes 1000 individual queries
    for (int i = 0; i < 1000; ++i) {
        std::string query = "INSERT INTO logs VALUES (" + std::to_string(i) + ")";
        PGresult* res = PQexec(conn, query.c_str());
        PQclear(res);
    }
}

void good_batch_insert(PGconn* conn) {
    // GOOD: Single batch insert with multiple values
    std::string query = "INSERT INTO logs VALUES ";
    for (int i = 0; i < 1000; ++i) {
        if (i > 0) query += ",";
        query += "(" + std::to_string(i) + ")";
    }
    PGresult* res = PQexec(conn, query.c_str());
    PQclear(res);
}

// Performance Characteristic 2: Prepared statements for repeated queries
void bad_repeated_queries(PGconn* conn) {
    // BAD: Parse and plan query each time
    for (int i = 0; i < 1000; ++i) {
        std::string query = "SELECT * FROM users WHERE id = " + std::to_string(i);
        PGresult* res = PQexec(conn, query.c_str());
        PQclear(res);
    }
}

void good_prepared_statement(PGconn* conn) {
    // GOOD: Prepare once, execute many times
    PGresult* prep = PQprepare(conn, "get_user", 
        "SELECT * FROM users WHERE id = $1", 1, nullptr);
    PQclear(prep);
    
    for (int i = 0; i < 1000; ++i) {
        const char* paramValues[1] = { std::to_string(i).c_str() };
        PGresult* res = PQexecPrepared(conn, "get_user", 1, paramValues, 
                                       nullptr, nullptr, 0);
        PQclear(res);
    }
}

// Performance Characteristic 3: COPY for bulk data loading
void bad_row_by_row_insert(PGconn* conn) {
    // BAD: Individual INSERT statements for bulk data
    for (int i = 0; i < 10000; ++i) {
        std::string query = "INSERT INTO data VALUES (" + 
                           std::to_string(i) + ", 'value_" + 
                           std::to_string(i) + "')";
        PGresult* res = PQexec(conn, query.c_str());
        PQclear(res);
    }
}

void good_copy_bulk_load(PGconn* conn) {
    // GOOD: Use COPY for bulk data loading
    PGresult* res = PQexec(conn, "COPY data FROM STDIN");
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        PQclear(res);
        return;
    }
    PQclear(res);
    
    // Send data in chunks
    std::string data;
    for (int i = 0; i < 10000; ++i) {
        data += std::to_string(i) + "\tvalue_" + std::to_string(i) + "\n";
        if (data.size() > 65536) {  // Send in 64KB chunks
            PQputCopyData(conn, data.c_str(), data.size());
            data.clear();
        }
    }
    if (!data.empty()) {
        PQputCopyData(conn, data.c_str(), data.size());
    }
    PQputCopyEnd(conn, nullptr);
    
    // Wait for completion
    PGresult* copy_result = PQgetResult(conn);
    PQclear(copy_result);
}

// Performance Characteristic 4: Connection pooling
class ConnectionPool {
public:
    ConnectionPool(const std::string& conninfo, size_t size) 
        : conninfo_(conninfo) {
        for (size_t i = 0; i < size; ++i) {
            pool_.push_back(createConnection());
        }
    }
    
    PGconn* acquire() {
        if (pool_.empty()) {
            return createConnection();
        }
        PGconn* conn = pool_.back();
        pool_.pop_back();
        return conn;
    }
    
    void release(PGconn* conn) {
        if (PQstatus(conn) == CONNECTION_OK) {
            pool_.push_back(conn);
        } else {
            PQfinish(conn);
        }
    }
    
private:
    PGconn* createConnection() {
        return PQconnectdb(conninfo_.c_str());
    }
    
    std::string conninfo_;
    std::vector<PGconn*> pool_;
};

// Performance Characteristic 5: Result streaming for large datasets
void bad_load_all_results(PGconn* conn) {
    // BAD: Loads entire result set into memory
    PGresult* res = PQexec(conn, "SELECT * FROM billion_row_table");
    // Memory usage: entire result set
    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        // Process each row
    }
    PQclear(res);
}

void good_streaming_results(PGconn* conn) {
    // GOOD: Use cursor for streaming
    PGresult* res = PQexec(conn, "DECLARE my_cursor CURSOR FOR SELECT * FROM billion_row_table");
    PQclear(res);
    
    while (true) {
        res = PQexec(conn, "FETCH 1000 FROM my_cursor");
        if (PQntuples(res) == 0) {
            PQclear(res);
            break;
        }
        // Process 1000 rows at a time
        PQclear(res);
    }
    
    res = PQexec(conn, "CLOSE my_cursor");
    PQclear(res);
}

// Performance measurement utility
template<typename Func>
double measure_performance(Func func, int iterations = 100) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count() / iterations;
}

int main() {
    PGconn* conn = PQconnectdb("host=localhost dbname=mydb");
    
    // Measure and compare performance
    double bad_time = measure_performance([&]() {
        bad_individual_queries(conn);
    }, 10);
    
    double good_time = measure_performance([&]() {
        good_batch_insert(conn);
    }, 10);
    
    std::cout << "Bad approach: " << bad_time << "s per iteration" << std::endl;
    std::cout << "Good approach: " << good_time << "s per iteration" << std::endl;
    std::cout << "Improvement: " << (bad_time / good_time) << "x faster" << std::endl;
    
    PQfinish(conn);
    return 0;
}
```