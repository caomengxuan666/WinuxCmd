# Threading

**Thread safety guarantees**
```cpp
// DoubleToStringConverter and StringToDoubleConverter are NOT thread-safe
// They contain mutable state that can be corrupted by concurrent access

// BAD: Shared converter across threads
double_conversion::DoubleToStringConverter global_converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);

void unsafe_thread_function() {
    char buffer[128];
    global_converter.ToShortest(3.14, buffer, sizeof(buffer)); // Data race!
}

// GOOD: Thread-local converter
thread_local double_conversion::DoubleToStringConverter tls_converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);

void safe_thread_function() {
    char buffer[128];
    tls_converter.ToShortest(3.14, buffer, sizeof(buffer)); // Safe
}
```

**Concurrent access patterns**
```cpp
// Pattern 1: Thread-local storage
class ThreadSafeConverter {
public:
    std::string convert(double value) {
        thread_local double_conversion::DoubleToStringConverter converter(
            double_conversion::DoubleToStringConverter::NO_FLAGS,
            "inf", "nan", 'e', 6, 6);
        thread_local char buffer[128];
        
        int length = converter.ToShortest(value, buffer, sizeof(buffer));
        return std::string(buffer, length);
    }
};

// Pattern 2: Mutex-protected converter
#include <mutex>

class MutexProtectedConverter {
public:
    std::string convert(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        char buffer[128];
        int length = converter_.ToShortest(value, buffer, sizeof(buffer));
        return std::string(buffer, length);
    }
    
private:
    std::mutex mutex_;
    double_conversion::DoubleToStringConverter converter_{
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6};
};
```

**Read-only sharing**
```cpp
// Converters can be shared read-only if properly synchronized
class ReadOnlyConverter {
public:
    ReadOnlyConverter() : converter_(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6) {}
    
    std::string convert(double value) const {
        // WARNING: This is NOT safe for concurrent access!
        // The converter modifies internal state during conversion
        char buffer[128];
        int length = converter_.ToShortest(value, buffer, sizeof(buffer));
        return std::string(buffer, length);
    }
    
private:
    // Even const methods modify the converter's internal state
    mutable double_conversion::DoubleToStringConverter converter_;
};
```

**Thread pool usage**
```cpp
#include <thread>
#include <vector>
#include <queue>
#include <mutex>

class ThreadPoolConverter {
public:
    ThreadPoolConverter(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back([this] { worker_thread(); });
        }
    }
    
    ~ThreadPoolConverter() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            done_ = true;
        }
        condition_.notify_all();
        for (auto& t : threads_) {
            t.join();
        }
    }
    
    void enqueue(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        condition_.notify_one();
    }
    
private:
    void worker_thread() {
        // Each thread has its own converter
        double_conversion::DoubleToStringConverter converter(
            double_conversion::DoubleToStringConverter::NO_FLAGS,
            "inf", "nan", 'e', 6, 6);
        char buffer[128];
        
        while (true) {
            double value;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this] {
                    return !queue_.empty() || done_;
                });
                
                if (done_ && queue_.empty()) break;
                
                value = queue_.front();
                queue_.pop();
            }
            
            int length = converter.ToShortest(value, buffer, sizeof(buffer));
            // Process result...
        }
    }
    
    std::vector<std::thread> threads_;
    std::queue<double> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool done_ = false;
};
```