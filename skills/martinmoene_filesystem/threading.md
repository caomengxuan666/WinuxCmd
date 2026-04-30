# Threading

```cpp
// Thread safety considerations for martinmoene/filesystem
#include "filesystem.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

namespace fs = ghc::filesystem;

// 1. Thread safety guarantees
// fs::path objects are NOT thread-safe for concurrent modification
// Filesystem operations are NOT atomic
// Directory iterators are NOT thread-safe

// 2. Thread-safe path operations
class ThreadSafePath {
public:
    void set_path(const fs::path& p) {
        std::lock_guard<std::mutex> lock(mutex_);
        path_ = p;
    }
    
    fs::path get_path() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return path_;
    }

private:
    mutable std::mutex mutex_;
    fs::path path_;
};

// 3. Thread-safe file operations
class ThreadSafeFileOperations {
public:
    bool safe_copy(const fs::path& from, const fs::path& to) {
        std::lock_guard<std::mutex> lock(operation_mutex_);
        std::error_code ec;
        return fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec);
    }
    
    bool safe_remove(const fs::path& p) {
        std::lock_guard<std::mutex> lock(operation_mutex_);
        std::error_code ec;
        return fs::remove(p, ec);
    }

private:
    std::mutex operation_mutex_;
};

// 4. Thread-safe directory iteration
class ThreadSafeDirectoryScanner {
public:
    std::vector<fs::path> scan_directory(const fs::path& dir) {
        std::lock_guard<std::mutex> lock(scan_mutex_);
        std::vector<fs::path> results;
        std::error_code ec;
        
        if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
            return results;
        }
        
        // Collect all entries atomically
        for (auto& entry : fs::directory_iterator(dir, ec)) {
            if (!ec) {
                results.push_back(entry.path());
            }
        }
        
        return results;
    }

private:
    std::mutex scan_mutex_;
};

// 5. Concurrent file access pattern
class ConcurrentFileAccess {
public:
    void write_file(const fs::path& p, const std::string& content) {
        std::lock_guard<std::mutex> lock(file_mutex_);
        std::ofstream file(p.string());
        file << content;
    }
    
    std::string read_file(const fs::path& p) {
        std::lock_guard<std::mutex> lock(file_mutex_);
        std::ifstream file(p.string());
        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        return content;
    }

private:
    std::mutex file_mutex_;
};

// 6. Thread-safe temporary file management
class ThreadSafeTempFile {
public:
    static fs::path create_temp() {
        std::lock_guard<std::mutex> lock(temp_mutex_);
        
        auto temp_dir = fs::temp_directory_path();
        fs::path temp_path;
        
        // Generate unique name
        static std::atomic<int> counter{0};
        temp_path = temp_dir / ("temp_" + std::to_string(++counter) + ".txt");
        
        std::ofstream(temp_path.string()) << "temporary";
        return temp_path;
    }
    
    static void cleanup_temp(const fs::path& p) {
        std::lock_guard<std::mutex> lock(temp_mutex_);
        std::error_code ec;
        fs::remove(p, ec);
    }

private:
    static std::mutex temp_mutex_;
};

std::mutex ThreadSafeTempFile::temp_mutex_;

// 7. Thread-safe directory monitoring
class ThreadSafeDirectoryMonitor {
public:
    void start_monitoring(const fs::path& dir) {
        std::lock_guard<std::mutex> lock(monitor_mutex_);
        monitored_dir_ = dir;
        running_ = true;
        monitor_thread_ = std::thread(&ThreadSafeDirectoryMonitor::monitor_loop, this);
    }
    
    void stop_monitoring() {
        running_ = false;
        if (monitor_thread_.joinable()) {
            monitor_thread_.join();
        }
    }
    
    std::vector<fs::path> get_changes() {
        std::lock_guard<std::mutex> lock(changes_mutex_);
        auto changes = std::move(pending_changes_);
        pending_changes_.clear();
        return changes;
    }

private:
    void monitor_loop() {
        std::error_code ec;
        std::vector<fs::path> previous_files;
        
        // Initial scan
        for (auto& entry : fs::directory_iterator(monitored_dir_, ec)) {
            previous_files.push_back(entry.path());
        }
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            std::vector<fs::path> current_files;
            for (auto& entry : fs::directory_iterator(monitored_dir_, ec)) {
                current_files.push_back(entry.path());
            }
            
            // Detect changes
            std::lock_guard<std::mutex> lock(changes_mutex_);
            for (auto& p : current_files) {
                if (std::find(previous_files.begin(), previous_files.end(), p) 
                    == previous_files.end()) {
                    pending_changes_.push_back(p);
                }
            }
            
            previous_files = std::move(current_files);
        }
    }
    
    std::thread monitor_thread_;
    std::atomic<bool> running_{false};
    fs::path monitored_dir_;
    std::mutex monitor_mutex_;
    std::mutex changes_mutex_;
    std::vector<fs::path> pending_changes_;
};

// 8. Example of thread-safe usage
void thread_safe_example() {
    ThreadSafeFileOperations file_ops;
    ThreadSafeDirectoryScanner scanner;
    std::vector<std::thread> threads;
    
    // Create test directory
    fs::path test_dir = "test_thread_safe";
    fs::create_directory(test_dir);
    
    // Create test files
    for (int i = 0; i < 10; ++i) {
        fs::path p = test_dir / ("file_" + std::to_string(i) + ".txt");
        std::ofstream(p.string()) << "content " << i;
    }
    
    // Concurrent operations
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&scanner, &test_dir]() {
            auto files = scanner.scan_directory(test_dir);
            std::cout << "Thread found " << files.size() << " files\n";
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Cleanup
    fs::remove_all(test_dir);
}
```