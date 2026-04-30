# Threading

**Thread Safety Guarantees**

RapidJSON's thread safety is limited:
- **Reading**: Multiple threads can read the same `Document` or `Value` concurrently
- **Writing**: No thread safety for writes - modifying a document from multiple threads is undefined behavior
- **SchemaDocument**: Not thread-safe for concurrent validation (see issues with heap corruption)

**Safe Concurrent Read Access**

```cpp
rapidjson::Document sharedDoc;
sharedDoc.Parse(jsonString);

// Multiple threads can read concurrently
std::vector<std::thread> readers;
for (int i = 0; i < 4; ++i) {
    readers.emplace_back([&sharedDoc]() {
        // Safe: only reading
        if (sharedDoc.HasMember("key")) {
            std::string val = sharedDoc["key"].GetString();
        }
    });
}
for (auto& t : readers) t.join();
```

**Thread-Safe Write Access with Mutex**

```cpp
#include <mutex>

class ThreadSafeDocument {
    rapidjson::Document doc;
    std::mutex mutex;
    
public:
    void update(const std::string& json) {
        std::lock_guard<std::mutex> lock(mutex);
        doc.Parse(json.c_str());
    }
    
    std::string getString(const char* key) {
        std::lock_guard<std::mutex> lock(mutex);
        if (doc.HasMember(key) && doc[key].IsString()) {
            return doc[key].GetString();
        }
        return "";
    }
};
```

**Per-Thread Document Pattern**

```cpp
// Each thread gets its own document
void worker(int id) {
    rapidjson::Document localDoc;
    localDoc.SetObject();
    localDoc.AddMember("thread_id", id, localDoc.GetAllocator());
    // Process with local document
}

std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back(worker, i);
}
for (auto& t : threads) t.join();
```

**SchemaDocument Thread Safety Workaround**

```cpp
// SchemaDocument is NOT thread-safe for concurrent validation
// Workaround: create per-thread copies

rapidjson::Document schemaDoc;
schemaDoc.Parse(schemaJson);

std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back([&schemaDoc]() {
        // Create local SchemaDocument for each thread
        rapidjson::SchemaDocument localSchema(schemaDoc);
        
        rapidjson::Document data;
        data.Parse(dataJson);
        
        rapidjson::SchemaValidator validator(localSchema);
        data.Accept(validator);
    });
}
for (auto& t : threads) t.join();
```

**Atomic Operations for Simple Values**

```cpp
#include <atomic>

std::atomic<int> sharedCounter{0};

// Thread-safe counter using atomic, not document
void incrementCounter() {
    sharedCounter.fetch_add(1, std::memory_order_relaxed);
}

// Update document with counter value (needs mutex)
std::mutex docMutex;
rapidjson::Document doc;

void updateDocument() {
    int val = sharedCounter.load();
    std::lock_guard<std::mutex> lock(docMutex);
    doc.SetObject();
    doc.AddMember("counter", val, doc.GetAllocator());
}
```