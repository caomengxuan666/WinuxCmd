# Threading

```cpp
// Threading: Thread safety guarantees and concurrent access patterns

// Thread safety guarantees:
// - rustls_connection: NOT thread-safe (single-threaded use only)
// - rustls_client_config: Thread-safe for read-only access
// - rustls_server_config: Thread-safe for read-only access
// - rustls_client_session_store: Thread-safe

// Pattern 1: Thread-local connections (correct)
#include <thread>
#include <vector>

void thread_safe_connections() {
    auto config = create_client_config();
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back([config]() {
            // Each thread gets its own connection
            auto conn = rustls_client_connection_new(config, "example.com");
            
            // Safe to use connection in this thread only
            const char* data = "Hello from thread";
            rustls_connection_write(conn, data, strlen(data));
            
            char buffer[1024];
            rustls_connection_read(conn, buffer, sizeof(buffer));
            
            rustls_connection_free(conn);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

// Pattern 2: Shared config, separate connections (correct)
void shared_config_threads() {
    // Config can be shared across threads (read-only)
    auto config = create_client_config();
    
    auto worker = [config](int id) {
        auto conn = rustls_client_connection_new(config, "example.com");
        // Safe: each thread has its own connection
        std::string msg = "Thread " + std::to_string(id);
        rustls_connection_write(conn, msg.c_str(), msg.size());
        rustls_connection_free(conn);
    };
    
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join();
    t2.join();
}

// Pattern 3: Thread-safe session store (correct)
void shared_session_store() {
    auto store = rustls_client_session_store_new();
    auto config = create_client_config();
    rustls_client_config_set_session_store(config, store);
    
    // Session store is thread-safe
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([config]() {
            auto conn = rustls_client_connection_new(config, "example.com");
            // Session resumption works across threads
            rustls_connection_free(conn);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    rustls_client_session_store_free(store);
}

// Pattern 4: Thread pool with connection ownership
class TlsThreadPool {
    rustls_client_config* config_;
    std::vector<std::thread> workers_;
    
public:
    void submit_task(const std::string& host, 
                     std::function<void(rustls_connection*)> task) {
        workers_.emplace_back([this, host, task]() {
            auto conn = rustls_client_connection_new(config_, host.c_str());
            task(conn);
            rustls_connection_free(conn);
        });
    }
    
    void wait_all() {
        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
    }
};

// Pattern 5: Mutex-protected connection (if sharing is necessary)
#include <mutex>

class SharedConnection {
    rustls_connection* conn_;
    std::mutex mutex_;
    
public:
    size_t write(const void* data, size_t len) {
        std::lock_guard<std::mutex> lock(mutex_);
        return rustls_connection_write(conn_, data, len);
    }
    
    size_t read(void* buffer, size_t len) {
        std::lock_guard<std::mutex> lock(mutex_);
        return rustls_connection_read(conn_, buffer, len);
    }
};

// Pattern 6: What NOT to do - sharing connection without synchronization
// BAD: Unsafe concurrent access
void bad_concurrent_access() {
    auto conn = rustls_client_connection_new(config, "host");
    
    std::thread writer([conn]() {
        rustls_connection_write(conn, "data", 4); // Data race!
    });
    
    std::thread reader([conn]() {
        char buffer[1024];
        rustls_connection_read(conn, buffer, sizeof(buffer)); // Data race!
    });
    
    writer.join();
    reader.join();
    rustls_connection_free(conn);
}
```