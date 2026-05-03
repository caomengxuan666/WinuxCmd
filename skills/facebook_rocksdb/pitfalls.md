# Pitfalls

## PITFALL 1: Ignoring Status return values

```cpp
// BAD: Ignoring status can hide silent data corruption
rocksdb::DB* db;
rocksdb::DB::Open(options, "/tmp/testdb", &db);  // Status ignored!
db->Put(rocksdb::WriteOptions(), "key", "value"); // May crash if Open failed

// GOOD: Always check Status
rocksdb::Status s = rocksdb::DB::Open(options, "/tmp/testdb", &db);
if (!s.ok()) {
    std::cerr << "Failed to open database: " << s.ToString() << std::endl;
    return;
}
s = db->Put(rocksdb::WriteOptions(), "key", "value");
if (!s.ok()) {
    std::cerr << "Put failed: " << s.ToString() << std::endl;
}
```

**Why it matters:** RocksDB errors are communicated entirely through `Status` objects. A failed `Open` leaves `db` as `nullptr`, so every subsequent call segfaults. Even a failed `Put` can indicate disk-full or corruption — silent writes lead to data loss in production.

## PITFALL 2: Not deleting iterators (memory leak)

```cpp
// BAD: Every NewIterator() call heap-allocates — missing delete = permanent leak
rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
it->SeekToFirst();
// ... use iterator but forget to delete
// → Memory is leaked every time this code runs

// GOOD: Always delete iterators when done
rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
it->SeekToFirst();
while (it->Valid()) {
    // process
    it->Next();
}
delete it;  // ★ Required — not optional
```

**Why it matters:** `DB::NewIterator()` returns a raw heap-allocated `Iterator*`. There is no RAII wrapper in the public API. In a long-running server, creating iterators without deletion will cause unbounded memory growth and eventually OOM. Always pair every `NewIterator()` with a matching `delete`. Prefer a `std::unique_ptr` with custom deleter or a scoped guard:

```cpp
auto it = std::unique_ptr<rocksdb::Iterator>(db->NewIterator(rocksdb::ReadOptions()));
```

## PITFALL 3: Using stale iterators after database close

```cpp
// BAD: Iterator holds a reference to DB internals — DB destroyed = dangling pointer
rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
delete db;            // DB memory freed, internal structures gone
it->SeekToFirst();    // ★ Undefined behavior — likely crash or garbage data

// GOOD: Destroy iterator before database
delete it;
delete db;
```

**Why it matters:** The iterator internally references the DB's memtable and SST file readers. When the DB is destroyed, those structures are freed. Any subsequent operation on the iterator reads freed memory — a classic use-after-free bug.

## PITFALL 4: Not setting create_if_missing for new databases

```cpp
// BAD: Opening non-existent database without create flag
rocksdb::Options opts;
// opts.create_if_missing = false; // default — will fail
rocksdb::DB* db;
rocksdb::Status s = rocksdb::DB::Open(opts, "/tmp/newdb", &db);
// s.IsNotFound() == true

// GOOD: Set create_if_missing for new databases
rocksdb::Options opts;
opts.create_if_missing = true;
rocksdb::DB* db;
rocksdb::Status s = rocksdb::DB::Open(opts, "/tmp/newdb", &db);
assert(s.ok());
// (or use error_if_exists=true if creating a fresh DB each run)
```

**Why it matters:** `create_if_missing` defaults to `false`. First-time runs on new machines will silently fail — a common cause of "works on my machine" bugs. Conversely, setting `error_if_exists = true` prevents accidentally overwriting a production database during development.

## PITFALL 5: Using default options for production

```cpp
// BAD: Default options may work for toy examples but fail under production load
rocksdb::Options options;  // All defaults — single-threaded compaction, 4MB write buffer
rocksdb::DB::Open(options, "/tmp/proddb", &db);
// → Will stall on writes with write-buffer pressure

// GOOD: Configure for production use
rocksdb::Options options;
options.IncreaseParallelism();              // Sets compaction/flush threads
options.OptimizeLevelStyleCompaction();     // Tunes for flash storage
options.create_if_missing = true;
options.write_buffer_size = 64 * 1024 * 1024; // 64MB (default is 4MB)
options.max_write_buffer_number = 4;           // (default is 2)
rocksdb::DB::Open(options, "/tmp/proddb", &db);
```

**Why it matters:** The default options (4MB write buffer, 1 compaction thread) are designed for embedded / resource-constrained scenarios. Using them without tuning on a production server leads to write stalls, high compaction debt, and unpredictable latency. Always call `IncreaseParallelism()` and set `write_buffer_size` based on your workload.

## PITFALL 6: Not handling WriteBatch errors

```cpp
// BAD: Assuming batch always succeeds
rocksdb::WriteBatch batch;
batch.Put("key1", "value1");
batch.Put("key2", "value2");
db->Write(rocksdb::WriteOptions(), &batch);  // Status ignored — lost error

// GOOD: Check batch write status
rocksdb::WriteBatch batch;
batch.Put("key1", "value1");
batch.Put("key2", "value2");
rocksdb::Status s = db->Write(rocksdb::WriteOptions(), &batch);
if (!s.ok()) {
    std::cerr << "Batch write failed: " << s.ToString() << std::endl;
    // Recovery: may need to retry or abort the transaction
}
```

**Why it matters:** `WriteBatch` is atomic — on failure, none of the operations are applied. But if you ignore the status, your application assumes the writes succeeded when they didn't, causing silent data loss.

## PITFALL 7: Iterating the entire DB when you only need a few keys

```cpp
// BAD: Full scan for a single key range
rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
for (it->SeekToFirst(); it->Valid(); it->Next()) {
    auto key = it->key().ToString();
    if (key.find("user:100") != std::string::npos) {
        // found it!
    }
}

// GOOD: Direct point lookup
std::string value;
rocksdb::Status s = db->Get(rocksdb::ReadOptions(), "user:100", &value);

// Or for range scans, use Seek() directly:
rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
for (it->Seek("user:100"); it->Valid() && it->key().starts_with("user:100"); it->Next()) {
    // process only keys in the "user:100*" prefix range
}
```

**Why it matters:** Full scans read and decompress every block from disk. For a 100GB database, a naive full scan may take minutes. LSM-tree SSTables are optimized for point lookups (via Bloom filters) and prefix scans — use them.
