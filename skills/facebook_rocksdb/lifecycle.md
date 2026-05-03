# Lifecycle

```cpp
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <memory>
#include <iostream>

// Construction: Opening a database
class DatabaseLifecycle {
private:
    rocksdb::DB* db_;
    std::string path_;
    
public:
    // Constructor opens database
    DatabaseLifecycle(const std::string& path) : db_(nullptr), path_(path) {
        rocksdb::Options options;
        options.create_if_missing = true;
        options.error_if_exists = false;
        
        rocksdb::Status s = rocksdb::DB::Open(options, path_, &db_);
        if (!s.ok()) {
            throw std::runtime_error("Failed to open database: " + s.ToString());
        }
    }
    
    // Destructor closes database
    ~DatabaseLifecycle() {
        Close();
    }
    
    // Move constructor - transfer ownership
    DatabaseLifecycle(DatabaseLifecycle&& other) noexcept 
        : db_(other.db_), path_(std::move(other.path_)) {
        other.db_ = nullptr;
    }
    
    // Move assignment
    DatabaseLifecycle& operator=(DatabaseLifecycle&& other) noexcept {
        if (this != &other) {
            Close();
            db_ = other.db_;
            path_ = std::move(other.path_);
            other.db_ = nullptr;
        }
        return *this;
    }
    
    // Delete copy operations
    DatabaseLifecycle(const DatabaseLifecycle&) = delete;
    DatabaseLifecycle& operator=(const DatabaseLifecycle&) = delete;
    
    // Explicit close with error handling
    void Close() {
        if (db_) {
            delete db_;
            db_ = nullptr;
        }
    }
    
    // Accessor
    rocksdb::DB* Get() { return db_; }
    
    // Check if database is valid
    bool IsValid() const { return db_ != nullptr; }
};

// Resource management with RAII
class ScopedIterator {
private:
    rocksdb::Iterator* iter_;
    
public:
    ScopedIterator(rocksdb::DB* db) 
        : iter_(db->NewIterator(rocksdb::ReadOptions())) {}
    
    ~ScopedIterator() {
        delete iter_;
    }
    
    // Move semantics
    ScopedIterator(ScopedIterator&& other) noexcept : iter_(other.iter_) {
        other.iter_ = nullptr;
    }
    
    ScopedIterator& operator=(ScopedIterator&& other) noexcept {
        if (this != &other) {
            delete iter_;
            iter_ = other.iter_;
            other.iter_ = nullptr;
        }
        return *this;
    }
    
    // Deleted copy
    ScopedIterator(const ScopedIterator&) = delete;
    ScopedIterator& operator=(const ScopedIterator&) = delete;
    
    rocksdb::Iterator* operator->() { return iter_; }
    rocksdb::Iterator& operator*() { return *iter_; }
};

// Usage example
void LifecycleExample() {
    // Construction
    DatabaseLifecycle db("/tmp/lifecycle_test");
    
    // Use the database
    rocksdb::Status s = db.Get()->Put(rocksdb::WriteOptions(), "key", "value");
    assert(s.ok());
    
    // Scoped iterator
    {
        ScopedIterator it(db.Get());
        it->SeekToFirst();
        while (it->Valid()) {
            std::cout << it->key().ToString() << std::endl;
            it->Next();
        }
    } // Iterator automatically deleted
    
    // Move ownership
    DatabaseLifecycle db2 = std::move(db);
    assert(!db.IsValid());
    assert(db2.IsValid());
    
    // Automatic cleanup in destructor
} // db2 destructor closes database
```