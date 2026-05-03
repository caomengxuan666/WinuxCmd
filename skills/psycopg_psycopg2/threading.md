# Threading

```cpp
#include <libpq-fe.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <condition_variable>

// IMPORTANT: libpq connections are NOT thread-safe by default
// Each connection should be used from only one thread at a time

// Thread Safety Pattern 1: One connection per thread
void thread_safe_per_thread() {
    auto worker = [](int thread_id) {
        // Each thread creates its own connection
        std::string conninfo = "host=localhost dbname=mydb user=myuser";
        PGconn* conn = PQconnectdb(conninfo.c_str());
        
        if (PQstatus(conn) != CONNECTION_OK) {
            std::cerr << "Thread " << thread_id << " connection failed" << std::endl;
            PQfinish(conn);
            return;
        }
        
        // Safe to use connection in this thread only
        PGresult* res = PQexec(conn, "SELECT 1");
        PQclear(res);
        
        PQfinish(conn);
    };
    
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    
    t1.join();
    t2.join();
}

// Thread Safety Pattern 2: Mutex-protected connection pool
class ThreadSafeConnectionPool {
public:
    ThreadSafeConnectionPool(const std::string& conninfo, size_t pool_size) 
        : conninfo_(conninfo), pool_size_(pool_size) {
        for (size_t i = 0; i < pool_size; ++i) {
            pool_.push(createConnection());
        }
    }
    
    PGconn* acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !pool_.empty(); });
        
        PGconn* conn = pool_.front();
        pool_.pop();
        return conn;
    }
    
    void release(PGconn* conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (PQstatus(conn) == CONNECTION_OK) {
            pool_.push(conn);
        } else {
            PQfinish(conn);
            pool_.push(createConnection());  // Replace broken connection
        }
        cv_.notify_one();
    }
    
private:
    PGconn* createConnection() {
        return PQconnectdb(conninfo_.c_str());
    }
    
    std::string conninfo_;
    size_t pool_size_;
    std::queue<PGconn*> pool_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// Thread Safety Pattern 3: Thread-local connections
class ThreadLocalConnection {
public:
    static PGconn* getConnection() {
        thread_local PGconn* conn = nullptr;
        if (!conn) {
            conn = PQconnectdb("host=localhost dbname=mydb user=myuser");
            if (PQstatus(conn) != CONNECTION_OK) {
                PQfinish(conn);
                conn = nullptr;
            }
        }
        return conn;
    }
    
    static void cleanup() {
        PGconn* conn = getConnection();
        if (conn) {
            PQfinish(conn);
            conn = nullptr;
        }
    }
};

// Thread Safety Pattern 4: Asynchronous operations with callbacks
class AsyncQueryExecutor {
public:
    using Callback = std::function<void(PGresult*)>;
    
    void executeAsync(const std::string& query, Callback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_queries_.push({query, callback});
    }
    
    void processResults() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!pending_queries_.empty()) {
            auto [query, callback] = pending_queries_.front();
            pending_queries_.pop();
            
            // This should be called from the connection's dedicated thread
            PGresult* res = PQexec(conn_, query.c_str());
            if (callback) {
                callback(res);
            }
            PQclear(res);
        }
    }
    
private:
    PGconn* conn_;
    std::mutex mutex_;
    std::queue<std::pair<std::string, Callback>> pending_queries_;
};

// Thread Safety Pattern 5: Connection ownership transfer
class ConnectionOwner {
public:
    ConnectionOwner(const std::string& conninfo) {
        conn_ = PQconnectdb(conninfo.c_str());
        owner_thread_ = std::this_thread::get_id();
    }
    
    void execute(const std::string& query) {
        // Check we're on the owning thread
        if (std::this_thread::get_id() != owner_thread_) {
            throw std::runtime_error("Connection used from wrong thread");
        }
        
        PGresult* res = PQexec(conn_, query.c_str());
        PQclear(res);
    }
    
    void transferOwnership() {
        owner_thread_ = std::this_thread::get_id();
    }
    
private:
    PGconn* conn_;
    std::thread::id owner_thread_;
};

// Thread Safety Pattern 6: Atomic operations for simple queries
class AtomicQueryExecutor {
public:
    AtomicQueryExecutor(const std::string& conninfo) {
        conn_ = PQconnectdb(conninfo.c_str());
    }
    
    void execute(const std::string& query) {
        std::lock_guard<std::mutex> lock(query_mutex_);
        PGresult* res = PQexec(conn_, query.c_str());
        PQclear(res);
    }
    
private:
    PGconn* conn_;
    std::mutex query_mutex_;
};

// Usage example demonstrating thread-safe patterns
int main() {
    // Pattern 1: One connection per thread
    std::cout << "Pattern 1: One connection per thread" << std::endl;
    thread_safe_per_thread();
    
    // Pattern 2: Thread-safe connection pool
    ThreadSafeConnectionPool pool("host=localhost dbname=mydb", 5);
    
    std::vector<std::thread> workers;
    for (int i = 0; i < 10; ++i) {
        workers.emplace_back([&pool, i]() {
            PGconn* conn = pool.acquire();
            
            // Use connection safely
            PGresult* res = PQexec(conn, "SELECT 1");
            PQclear(res);
            
            pool.release(conn);
        });
    }
    
    for (auto& t : workers) {
        t.join();
    }
    
    // Pattern 3: Thread-local connections
    std::thread t1([]() {
        PGconn* conn = ThreadLocalConnection::getConnection();
        if (conn) {
            PGresult* res = PQexec(conn, "SELECT 1");
            PQclear(res);
        }
    });
    
    std::thread t2([]() {
        PGconn* conn = ThreadLocalConnection::getConnection();
        if (conn) {
            PGresult* res = PQexec(conn, "SELECT 2");
            PQclear(res);
        }
    });
    
    t1.join();
    t2.join();
    
    return 0;
}
```