# Safety

```cpp
// RED LINE 1: Never use a database after it has been closed
// BAD: Using closed database
rocksdb::DB* db;
rocksdb::DB::Open(options, "/tmp/testdb", &db);
delete db;
db->Put(rocksdb::WriteOptions(), "key", "value");  // CRASH!

// GOOD: Set pointer to nullptr after deletion
delete db;
db = nullptr;
// Check before use
if (db) {
    db->Put(rocksdb::WriteOptions(), "key", "value");
}

// RED LINE 2: Never access iterators after their parent database is destroyed
// BAD: Iterator outlives database
rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
delete db;
it->SeekToFirst();  // CRASH!

// GOOD: Destroy iterator before database
delete it;
delete db;

// RED LINE 3: Never ignore Status return values from critical operations
// BAD: Ignoring corruption status
rocksdb::Status s = db->Put(rocksdb::WriteOptions(), "key", "value");
// s.IsCorruption() may be true, but we ignore it

// GOOD: Always check and handle critical status
rocksdb::Status s = db->Put(rocksdb::WriteOptions(), "key", "value");
if (s.IsCorruption()) {
    // Handle corruption, possibly reopen database
    std::cerr << "Database corruption detected!" << std::endl;
    // Recovery logic here
}

// RED LINE 4: Never use the same database instance from multiple threads without synchronization
// BAD: Concurrent writes without synchronization
// Thread 1
db->Put(rocksdb::WriteOptions(), "key1", "value1");
// Thread 2 (simultaneously)
db->Put(rocksdb::WriteOptions(), "key1", "value2");  // Race condition!

// GOOD: Use separate database instances or external synchronization
std::mutex mtx;
// Thread 1
{
    std::lock_guard<std::mutex> lock(mtx);
    db->Put(rocksdb::WriteOptions(), "key1", "value1");
}
// Thread 2
{
    std::lock_guard<std::mutex> lock(mtx);
    db->Put(rocksdb::WriteOptions(), "key1", "value2");
}

// RED LINE 5: Never use internal APIs that may change without warning
// BAD: Using internal header
#include <rocksdb/internal/some_internal_api.h>  // May break in next version

// GOOD: Only use public API from include/ directory
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/write_batch.h>
```