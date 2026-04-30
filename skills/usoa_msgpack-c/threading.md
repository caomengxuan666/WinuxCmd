# Threading

```cpp
// Thread safety patterns with msgpack-c
#include <msgpack.hpp>
#include <thread>
#include <mutex>
#include <atomic>

// Thread Safety Pattern 1: Thread-local sbuffer for serialization
void thread_local_serialization() {
    thread_local msgpack::sbuffer local_buffer;
    
    auto worker = []() {
        local_buffer.clear();
        msgpack::pack(local_buffer, 42);
        // Each thread has its own buffer, no synchronization needed
    };
    
    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();
}

// Thread Safety Pattern 2: Mutex-protected shared buffer
class SharedBuffer {
    msgpack::sbuffer buffer_;
    std::mutex mutex_;
    
public:
    template <typename T>
    void pack(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        msgpack::pack(buffer_, value);
    }
    
    std::string get_data() {
        std::lock_guard<std::mutex> lock(mutex_);
        return std::string(buffer_.data(), buffer_.size());
    }
};

void shared_buffer_example() {
    SharedBuffer shared;
    
    std::thread writer1([&shared]() {
        shared.pack(1);
    });
    
    std::thread writer2([&shared]() {
        shared.pack(2);
    });
    
    writer1.join();
    writer2.join();
    
    std::string data = shared.get_data();
}

// Thread Safety Pattern 3: Read-only access to unpacked data
class ReadOnlyCache {
    msgpack::object_handle handle_;
    std::shared_mutex mutex_;
    
public:
    void update(const std::string& data) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        handle_ = msgpack::unpack(data.data(), data.size());
    }
    
    template <typename T>
    T read() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return handle_.get().as<T>();
    }
};

// Thread Safety Pattern 4: Atomic counter for message IDs
std::atomic<uint64_t> message_id_counter{0};

void atomic_message_id() {
    auto worker = []() {
        uint64_t id = message_id_counter.fetch_add(1);
        
        msgpack::sbuffer buffer;
        msgpack::pack(buffer, id);
        // Each message gets unique ID without locks
    };
    
    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();
}

// Thread Safety Pattern 5: Thread-safe object pool
class ObjectPool {
    std::vector<msgpack::sbuffer> buffers_;
    std::mutex mutex_;
    std::condition_variable cv_;
    
public:
    ObjectPool(size_t size) : buffers_(size) {}
    
    msgpack::sbuffer acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !buffers_.empty(); });
        
        auto buffer = std::move(buffers_.back());
        buffers_.pop_back();
        return buffer;
    }
    
    void release(msgpack::sbuffer buffer) {
        buffer.clear();
        std::unique_lock<std::mutex> lock(mutex_);
        buffers_.push_back(std::move(buffer));
        cv_.notify_one();
    }
};

// Thread Safety Pattern 6: Thread-safe deserialization
class ThreadSafeDeserializer {
    msgpack::unpacker pac_;
    std::mutex mutex_;
    
public:
    void feed(const char* data, size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        pac_.reserve_buffer(size);
        memcpy(pac_.buffer(), data, size);
        pac_.buffer_consumed(size);
    }
    
    bool try_get(msgpack::unpacked& result) {
        std::lock_guard<std::mutex> lock(mutex_);
        return pac_.next(result);
    }
};

// Thread Safety Pattern 7: Immutable data sharing
class ImmutableMessage {
    std::shared_ptr<const msgpack::object_handle> data_;
    
public:
    ImmutableMessage(std::string serialized)
        : data_(std::make_shared<msgpack::object_handle>(
            msgpack::unpack(serialized.data(), serialized.size()))) {}
    
    template <typename T>
    T get() const {
        return data_->get().as<T>();
    }
};

void immutable_sharing() {
    auto msg = std::make_shared<ImmutableMessage>(
        []() {
            msgpack::sbuffer buf;
            msgpack::pack(buf, 42);
            return std::string(buf.data(), buf.size());
        }()
    );
    
    // Multiple threads can read safely
    std::thread reader1([msg]() { int v = msg->get<int>(); });
    std::thread reader2([msg]() { int v = msg->get<int>(); });
    
    reader1.join();
    reader2.join();
}
```