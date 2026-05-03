# Quickstart

This quickstart shows how to integrate RocksDB into a C++ project from scratch — with proper includes, error handling, cleanup, and a production-ready skeleton.

## Prerequisites

RocksDB is a header-only compatible library. Install via:

```bash
# Ubuntu/Debian
sudo apt-get install librocksdb-dev

# macOS (Homebrew)
brew install rocksdb

# Or build from source
git clone https://github.com/facebook/rocksdb.git
cd rocksdb && make shared_lib && sudo make install
```

Link with `-lrocksdb` (or `./bcmake build` and link the static lib).

## Minimal Open / Put / Get / Close

```cpp
#include <iostream>
#include <cassert>
#include <rocksdb/db.h>
#include <rocksdb/options.h>

int main() {
    // 1. Open (or create) the database
    rocksdb::DB* db = nullptr;
    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = false;

    rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/rocksdb_demo", &db);
    if (!status.ok()) {
        std::cerr << "Failed to open DB: " << status.ToString() << std::endl;
        return 1;
    }

    // 2. Write a key-value pair
    status = db->Put(rocksdb::WriteOptions(), "greeting", "Hello, RocksDB!");
    if (!status.ok()) {
        std::cerr << "Put failed: " << status.ToString() << std::endl;
        delete db;
        return 1;
    }

    // 3. Read the value back
    std::string value;
    status = db->Get(rocksdb::ReadOptions(), "greeting", &value);
    if (status.ok()) {
        std::cout << "Read: " << value << std::endl;
    } else if (status.IsNotFound()) {
        std::cout << "Key not found" << std::endl;
    } else {
        std::cerr << "Get failed: " << status.ToString() << std::endl;
    }

    // 4. Delete the key
    status = db->Delete(rocksdb::WriteOptions(), "greeting");
    if (!status.ok()) {
        std::cerr << "Delete failed: " << status.ToString() << std::endl;
    }

    // 5. Clean up — always delete the DB handle
    delete db;
    return 0;
}
```

## Iterating Over Keys

```cpp
#include <iostream>
#include <rocksdb/db.h>
#include <rocksdb/options.h>

void iterate_all(rocksdb::DB* db) {
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    if (!it) {
        std::cerr << "Failed to create iterator" << std::endl;
        return;
    }

    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::cout << it->key().ToString() << ": " << it->value().ToString() << std::endl;
    }

    if (!it->status().ok()) {
        std::cerr << "Iterator error: " << it->status().ToString() << std::endl;
    }

    // ★ Always delete iterators — they are heap-allocated
    delete it;
}
```

## Atomic Batch Writes

```cpp
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>

rocksdb::Status batch_write(rocksdb::DB* db) {
    rocksdb::WriteBatch batch;
    batch.Put("user:1", "Alice");
    batch.Put("user:2", "Bob");
    batch.Put("user:3", "Charlie");
    batch.Delete("user:0");

    rocksdb::Status s = db->Write(rocksdb::WriteOptions(), &batch);
    if (!s.ok()) {
        std::cerr << "Batch write failed: " << s.ToString() << std::endl;
    }
    return s;  // batch either fully applied or fully rolled back
}
```

## Production-Ready Skeleton

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <stdexcept>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/iterator.h>

class RocksDBStore {
public:
    explicit RocksDBStore(const std::string& db_path) {
        rocksdb::Options opts;
        opts.create_if_missing = true;
        opts.IncreaseParallelism();
        opts.OptimizeLevelStyleCompaction();

        rocksdb::DB* raw = nullptr;
        rocksdb::Status s = rocksdb::DB::Open(opts, db_path, &raw);
        if (!s.ok()) {
            throw std::runtime_error("RocksDB open failed: " + s.ToString());
        }
        db_.reset(raw);
    }

    void Put(const std::string& key, const std::string& value) {
        rocksdb::Status s = db_->Put(rocksdb::WriteOptions(), key, value);
        if (!s.ok()) {
            throw std::runtime_error("RocksDB put failed: " + s.ToString());
        }
    }

    std::string Get(const std::string& key) {
        std::string val;
        rocksdb::Status s = db_->Get(rocksdb::ReadOptions(), key, &val);
        if (s.IsNotFound()) return "";
        if (!s.ok()) throw std::runtime_error("RocksDB get failed: " + s.ToString());
        return val;
    }

    void Delete(const std::string& key) {
        rocksdb::Status s = db_->Delete(rocksdb::WriteOptions(), key);
        if (!s.ok()) {
            throw std::runtime_error("RocksDB delete failed: " + s.ToString());
        }
    }

private:
    // Custom deleter: DB::Close() is deprecated; delete db pointer directly
    struct DBDeleter {
        void operator()(rocksdb::DB* db) { delete db; }
    };
    std::unique_ptr<rocksdb::DB, DBDeleter> db_;
};

// Usage
int main() {
    try {
        RocksDBStore store("/tmp/rocksdb_production");

        store.Put("key1", "hello");
        std::cout << store.Get("key1") << std::endl;  // "hello"
        store.Delete("key1");
        std::cout << "'" << store.Get("key1") << "'" << std::endl;  // ""

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

## Next Steps

- Read [overview.md](overview.md) for the LSM-tree architecture
- Read [pitfalls.md](pitfalls.md) for common mistakes to avoid
- Read [lifecycle.md](lifecycle.md) for RAII patterns and resource management
