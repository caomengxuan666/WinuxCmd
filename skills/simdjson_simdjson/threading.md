# Threading

**Thread Safety Guarantees**

simdjson provides the following thread safety guarantees:

```cpp
// 1. Parser objects are NOT thread-safe
// Each thread must have its own parser
void thread_unsafe_example() {
    ondemand::parser shared_parser;
    
    std::thread t1([&]() {
        auto json = padded_string::load("file1.json");
        auto doc = shared_parser.iterate(json); // Race condition!
    });
    
    std::thread t2([&]() {
        auto json = padded_string::load("file2.json");
        auto doc = shared_parser.iterate(json); // Race condition!
    });
}

// 2. Correct: Each thread has its own parser
void thread_safe_example() {
    std::thread t1([]() {
        ondemand::parser parser;
        auto json = padded_string::load("file1.json");
        auto doc = parser.iterate(json);
        process(doc);
    });
    
    std::thread t2([]() {
        ondemand::parser parser;
        auto json = padded_string::load("file2.json");
        auto doc = parser.iterate(json);
        process(doc);
    });
}
```

**Concurrent Access Patterns**

```cpp
// 1. Thread pool for parallel file processing
void parallel_file_processing(const std::vector<std::string>& files) {
    std::vector<std::thread> threads;
    
    for (const auto& file : files) {
        threads.emplace_back([file]() {
            ondemand::parser parser;
            padded_string json = padded_string::load(file);
            auto doc = parser.iterate(json);
            process_document(doc);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

// 2. Producer-consumer with padded_string
void producer_consumer() {
    std::mutex mtx;
    std::queue<padded_string> queue;
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < 100; ++i) {
            padded_string json = padded_string::load("data_" + std::to_string(i) + ".json");
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(std::move(json));
        }
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        ondemand::parser parser;
        while (true) {
            padded_string json;
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (queue.empty()) break;
                json = std::move(queue.front());
                queue.pop();
            }
            auto doc = parser.iterate(json);
            process_document(doc);
        }
    });
    
    producer.join();
    consumer.join();
}
```

**Read-Only Shared Data**

```cpp
// padded_string data can be shared read-only across threads
void shared_read_only() {
    padded_string json = padded_string::load("shared.json");
    
    // Multiple threads can read the same data
    std::thread t1([&json]() {
        ondemand::parser parser;
        auto doc = parser.iterate(json);
        auto val1 = doc["field1"].get_string();
    });
    
    std::thread t2([&json]() {
        ondemand::parser parser;
        auto doc = parser.iterate(json);
        auto val2 = doc["field2"].get_string();
    });
    
    t1.join();
    t2.join();
}
```

**Thread-Local Storage for Parsers**

```cpp
// Use thread_local for frequently used parsers
class ThreadSafeProcessor {
    thread_local static ondemand::parser parser;
    
public:
    void process(const std::string& filename) {
        padded_string json = padded_string::load(filename);
        auto doc = parser.iterate(json); // Thread-local parser
        process_document(doc);
    }
};

// Initialize thread_local parser
thread_local ondemand::parser ThreadSafeProcessor::parser;
```