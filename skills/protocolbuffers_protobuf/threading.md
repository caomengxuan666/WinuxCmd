# Threading

### Thread safety guarantees
```cpp
// Protobuf messages are NOT thread-safe for mutation
// Thread 1:
msg.mutable_person()->set_name("Alice");
// Thread 2 (concurrent):
msg.mutable_person()->set_age(30);  // Data race!

// Solution: Use mutex
std::mutex mtx;
// Thread 1:
{
    std::lock_guard<std::mutex> lock(mtx);
    msg.mutable_person()->set_name("Alice");
}
// Thread 2:
{
    std::lock_guard<std::mutex> lock(mtx);
    msg.mutable_person()->set_age(30);
}
```

### Read-only concurrent access
```cpp
// Safe: Multiple readers can access const reference
const Person& person = GetPerson();

// Thread 1:
std::cout << person.name();  // Safe - read only

// Thread 2:
std::cout << person.email();  // Safe - read only

// Thread 3:
// person.mutable_name()->append(" Jr.");  // NOT safe - would need mutex
```

### Copy-on-write pattern
```cpp
// Good: Copy then modify
std::mutex mtx;
Person shared_person;

void UpdatePerson(const std::string& new_name) {
    Person local_copy;
    {
        std::lock_guard<std::mutex> lock(mtx);
        local_copy = shared_person;  // Thread-safe copy
    }
    local_copy.set_name(new_name);
    {
        std::lock_guard<std::mutex> lock(mtx);
        shared_person = std::move(local_copy);  // Thread-safe update
    }
}
```

### Thread-safe arena allocation
```cpp
// Arena is NOT thread-safe for allocation
google::protobuf::Arena arena;

// BAD: Concurrent allocation on same arena
// Thread 1:
auto* p1 = google::protobuf::Arena::CreateMessage<Person>(&arena);
// Thread 2:
auto* p2 = google::protobuf::Arena::CreateMessage<Person>(&arena);  // Data race!

// GOOD: Per-thread arenas
thread_local google::protobuf::Arena thread_arena;

void ProcessThread() {
    auto* msg = google::protobuf::Arena::CreateMessage<MyMessage>(&thread_arena);
    // Safe - each thread has its own arena
}
```

### Thread-safe serialization
```cpp
// Serialization is read-only, so it's thread-safe
const Person person = GetPerson();

// Thread 1:
std::string data1;
person.SerializeToString(&data1);  // Safe

// Thread 2:
std::string data2;
person.SerializeToString(&data2);  // Safe

// But NOT safe if person is being modified concurrently
```

### Thread pool with message ownership
```cpp
// Good: Pass ownership between threads
class ThreadPool {
    std::queue<std::unique_ptr<MyMessage>> queue;
    std::mutex mtx;
    
public:
    void Enqueue(std::unique_ptr<MyMessage> msg) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(std::move(msg));
    }
    
    std::unique_ptr<MyMessage> Dequeue() {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return nullptr;
        auto msg = std::move(queue.front());
        queue.pop();
        return msg;
    }
};
```