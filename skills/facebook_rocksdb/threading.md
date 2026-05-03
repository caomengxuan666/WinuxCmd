# Threading

```cpp
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

// Thread safety: RocksDB provides some built-in thread safety
// Multiple threads can read concurrently
// Writes are serialized internally

// Thread-safe database wrapper
class ThreadSafeDatabase {
private:
    rocksdb::DB* db_;
    std::mutex write_mutex_;  // For write serialization
    std::shared_mutex read_mutex_;  // For read coordination
    
public:
    ThreadSafeDatabase(const std::string& path) {
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::Status s = rocksdb::DB::Open(options, path, &db_);
        assert(s.ok());
    }
    
    ~ThreadSafeDatabase() {
        delete db_;
    }
    
    // Thread-safe read
    bool Get(const std::string& key, std::string& value) {
        std::shared_lock<std::shared_mutex> lock(read_mutex_);
        rocksdb::Status s = db_->Get(rocksdb::ReadOptions(), key, &value);
        return s.ok();
    }
    
    // Thread-safe write with mutex
    bool Put(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(write_mutex_);
        rocksdb::Status s = db_->Put(rocksdb::WriteOptions(), key, &value);
        return s.ok();
    }
    
    // Thread-safe batch write
    bool WriteBatch(const std::vector<std::pair<std::string, std::string>>& entries) {
        std::lock_guard<std::mutex> lock(write_mutex_);
        rocksdb::WriteBatch batch;
        for (const auto& entry : entries) {
            batch.Put(entry.first, entry.second);
        }
        rocksdb::Status s = db_->Write(rocksdb::WriteOptions(), &batch);
        return s.ok();
    }
};

// Concurrent read example
void ConcurrentReadExample(rocksdb::DB* db, int num_threads) {
    std::vector<std::thread> threads;
    std::atomic<int> successful_reads{0};
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([db, &successful_reads, i]() {
            std::string value;
            rocksdb::ReadOptions options;
            options.verify_checksums = false;  // Skip checksums for performance
            
            // Multiple threads can read concurrently
            for (int j = 0; j < 100; ++j) {
                std::string key = "key_" + std::to_string(j);
                rocksdb::Status s = db->Get(options, key, &value);
                if (s.ok()) {
                    successful_reads++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Successful reads: " << successful_reads << std::endl;
}

// Concurrent write example with proper synchronization
void ConcurrentWriteExample(rocksdb::DB* db, int num_threads) {
    std::vector<std::thread> threads;
    std::mutex write_mutex;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([db, &write_mutex, i]() {
            // Writes should be synchronized externally
            std::lock_guard<std::mutex> lock(write_mutex);
            
            rocksdb::WriteOptions options;
            options.sync = false;  // Disable fsync for performance
            
            std::string key = "thread_" + std::to_string(i) + "_key";
            std::string value = "thread_" + std::to_string(i) + "_value";
            
            rocksdb::Status s = db->Put(options, key, value);
            if (!s.ok()) {
                std::cerr << "Write failed: " << s.ToString() << std::endl;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

// Thread-safe iterator usage
class ThreadSafeIterator {
private:
    rocksdb::DB* db_;
    std::mutex iter_mutex_;
    
public:
    ThreadSafeIterator(rocksdb::DB* db) : db_(db) {}
    
    void IterateRange(const std::string& start, const std::string& end) {
        std::lock_guard<std::mutex> lock(iter_mutex_);
        
        rocksdb::ReadOptions options;
        options.iterate_upper_bound = new rocksdb::Slice(end);
        
        rocksdb::Iterator* it = db_->NewIterator(options);
        it->Seek(start);
        
        while (it->Valid()) {
            // Process key-value
            it->Next();
        }
        
        delete it;
        delete options.iterate_upper_bound;
    }
};
```