# Best Practices

```cpp
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/table.h>
#include <memory>
#include <iostream>

// Best Practice 1: Use smart pointers for automatic cleanup
class DatabaseManager {
private:
    std::unique_ptr<rocksdb::DB, decltype(&rocksdb::DB::Close)> db_;
    
public:
    DatabaseManager() : db_(nullptr, rocksdb::DB::Close) {}
    
    bool Open(const std::string& path) {
        rocksdb::DB* raw_db = nullptr;
        rocksdb::Options options;
        options.create_if_missing = true;
        options.IncreaseParallelism();
        options.OptimizeLevelStyleCompaction();
        
        rocksdb::Status s = rocksdb::DB::Open(options, path, &raw_db);
        if (s.ok()) {
            db_.reset(raw_db);
            return true;
        }
        return false;
    }
};

// Best Practice 2: Configure write buffers for workload
rocksdb::Options ConfigureForWriteHeavy() {
    rocksdb::Options options;
    options.create_if_missing = true;
    options.write_buffer_size = 128 * 1024 * 1024;  // 128MB
    options.max_write_buffer_number = 6;
    options.min_write_buffer_number_to_merge = 2;
    options.max_background_compactions = 4;
    options.max_background_flushes = 2;
    return options;
}

// Best Practice 3: Use column families for logical separation
class MultiColumnDatabase {
private:
    rocksdb::DB* db_;
    rocksdb::ColumnFamilyHandle* default_cf_;
    rocksdb::ColumnFamilyHandle* metadata_cf_;
    
public:
    bool Open(const std::string& path) {
        rocksdb::DBOptions db_options;
        db_options.create_if_missing = true;
        db_options.create_missing_column_families = true;
        
        std::vector<rocksdb::ColumnFamilyDescriptor> cf_descs;
        cf_descs.push_back(rocksdb::ColumnFamilyDescriptor(
            rocksdb::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions()));
        cf_descs.push_back(rocksdb::ColumnFamilyDescriptor(
            "metadata", rocksdb::ColumnFamilyOptions()));
        
        std::vector<rocksdb::ColumnFamilyHandle*> handles;
        rocksdb::Status s = rocksdb::DB::Open(db_options, path, cf_descs, &handles, &db_);
        if (s.ok()) {
            default_cf_ = handles[0];
            metadata_cf_ = handles[1];
            return true;
        }
        return false;
    }
    
    void Close() {
        delete metadata_cf_;
        delete default_cf_;
        delete db_;
    }
};

// Best Practice 4: Use WriteBatch for atomic operations
bool AtomicUpdate(rocksdb::DB* db, const std::string& key, 
                  const std::string& old_value, const std::string& new_value) {
    rocksdb::WriteBatch batch;
    batch.Put(key, new_value);
    
    // Use merge operator for atomic read-modify-write if available
    rocksdb::Status s = db->Write(rocksdb::WriteOptions(), &batch);
    return s.ok();
}

// Best Practice 5: Implement proper error handling with retries
bool RobustPut(rocksdb::DB* db, const std::string& key, const std::string& value, 
               int max_retries = 3) {
    for (int i = 0; i < max_retries; ++i) {
        rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, value);
        if (s.ok()) return true;
        
        if (s.IsTimedOut() || s.IsTryAgain()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100 * (i + 1)));
            continue;
        }
        return false;
    }
    return false;
}
```