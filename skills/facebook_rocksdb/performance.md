# Performance

```cpp
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/table.h>
#include <rocksdb/filter_policy.h>
#include <chrono>
#include <iostream>

// Performance optimization: Configure for read-heavy workloads
rocksdb::Options ConfigureForReadHeavy() {
    rocksdb::Options options;
    options.create_if_missing = true;
    
    // Use Bloom filters for faster point lookups
    rocksdb::BlockBasedTableOptions table_options;
    table_options.filter_policy.reset(rocksdb::NewBloomFilterPolicy(10, false));
    table_options.block_size = 16 * 1024; // 16KB blocks
    options.table_factory.reset(rocksdb::NewBlockBasedTableFactory(table_options));
    
    // Cache configuration
    options.block_cache = rocksdb::NewLRUCache(512 * 1024 * 1024); // 512MB cache
    options.max_open_files = 1000;
    
    return options;
}

// Performance optimization: Batch writes for throughput
void BatchWriteExample(rocksdb::DB* db, int num_keys) {
    auto start = std::chrono::high_resolution_clock::now();
    
    rocksdb::WriteOptions write_options;
    write_options.disableWAL = false; // Enable WAL for durability
    
    // Batch writes in groups of 1000
    const int batch_size = 1000;
    for (int i = 0; i < num_keys; i += batch_size) {
        rocksdb::WriteBatch batch;
        for (int j = i; j < std::min(i + batch_size, num_keys); ++j) {
            batch.Put("key_" + std::to_string(j), "value_" + std::to_string(j));
        }
        rocksdb::Status s = db->Write(write_options, &batch);
        assert(s.ok());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Wrote " << num_keys << " keys in " << duration.count() << "ms" << std::endl;
}

// Performance optimization: Use iterators efficiently
void EfficientIteration(rocksdb::DB* db) {
    rocksdb::ReadOptions read_options;
    read_options.verify_checksums = false; // Skip checksums for read-only workloads
    read_options.fill_cache = true; // Fill block cache
    
    rocksdb::Iterator* it = db->NewIterator(read_options);
    
    // Seek to specific key instead of iterating from start
    it->Seek("key_500");
    
    // Iterate only needed range
    int count = 0;
    while (it->Valid() && count < 100) {
        // Process key-value
        count++;
        it->Next();
    }
    
    delete it;
}

// Performance optimization: Memory allocation patterns
void MemoryEfficientOperations(rocksdb::DB* db) {
    // Reuse string objects to avoid allocations
    std::string key;
    std::string value;
    
    for (int i = 0; i < 1000; ++i) {
        key = "key_" + std::to_string(i);
        value = "value_" + std::to_string(i);
        
        rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, value);
        assert(s.ok());
    }
}

// Performance optimization: Compaction tuning
rocksdb::Options ConfigureCompaction() {
    rocksdb::Options options;
    options.create_if_missing = true;
    
    // Level compaction for balanced performance
    options.compaction_style = rocksdb::kCompactionStyleLevel;
    options.level0_file_num_compaction_trigger = 4;
    options.level0_slowdown_writes_trigger = 8;
    options.level0_stop_writes_trigger = 12;
    
    // Parallel compaction
    options.max_background_compactions = 4;
    options.max_subcompactions = 2;
    
    return options;
}
```