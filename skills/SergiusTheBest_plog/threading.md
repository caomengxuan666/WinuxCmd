# Threading


Thread safety guarantees and concurrent access patterns in Plog.

**Basic thread safety**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <thread>
#include <vector>

void worker(int id) {
    for (int i = 0; i < 100; ++i) {
        PLOGI << "Thread " << id << " iteration " << i;
    }
}

int main() {
    plog::init(plog::debug, "threaded.log");
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    // All messages are safely written without interleaving
    return 0;
}
```

**Thread-safe custom appender**
```cpp
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <mutex>
#include <queue>

class ThreadSafeQueueAppender : public plog::IAppender {
    std::queue<std::string> messages;
    mutable std::mutex mutex;
public:
    void write(const plog::Record& record) override {
        std::lock_guard<std::mutex> lock(mutex);
        messages.push(record.getMessage());
    }
    
    std::string pop() {
        std::lock_guard<std::mutex> lock(mutex);
        if (messages.empty()) return "";
        auto msg = messages.front();
        messages.pop();
        return msg;
    }
};

int main() {
    static ThreadSafeQueueAppender appender;
    plog::init(plog::debug, &appender);
    
    std::thread t1([] { PLOGD << "From thread 1"; });
    std::thread t2([] { PLOGD << "From thread 2"; });
    t1.join();
    t2.join();
    
    std::cout << appender.pop() << std::endl;
    std::cout << appender.pop() << std::endl;
    return 0;
}
```

**Per-thread logging with multiple loggers**
```cpp
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <thread>

void threadWorker(int threadId) {
    // Each thread gets its own logger instance
    static plog::RollingFileAppender<plog::TxtFormatter> appender(
        ("thread_" + std::to_string(threadId) + ".log").c_str()
    );
    plog::init<threadId + 1>(plog::debug, &appender);
    
    PLOGD_(threadId + 1) << "Running on thread " << threadId;
}

int main() {
    std::thread t1(threadWorker, 1);
    std::thread t2(threadWorker, 2);
    t1.join();
    t2.join();
    return 0;
}
```

**Thread safety with severity changes**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <thread>
#include <atomic>

std::atomic<bool> running{true};

void logWorker() {
    while (running) {
        PLOGD << "Debug message";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void severityChanger() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    plog::get()->setMaxSeverity(plog::info); // Thread-safe
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    plog::get()->setMaxSeverity(plog::debug); // Thread-safe
    running = false;
}

int main() {
    plog::init(plog::debug, "app.log");
    
    std::thread worker(logWorker);
    std::thread changer(severityChanger);
    
    worker.join();
    changer.join();
    return 0;
}
```

**Avoiding deadlocks in signal handlers**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <csignal>
#include <atomic>

std::atomic<bool> inSignalHandler{false};

void signalHandler(int) {
    inSignalHandler = true;
    // BAD: logging in signal handler may deadlock if mutex is held
    // PLOGE << "Signal received"; // Potential deadlock
    
    // GOOD: use a simple flag and log from main thread
    inSignalHandler = false;
}

int main() {
    plog::init(plog::debug, "app.log");
    std::signal(SIGINT, signalHandler);
    
    while (true) {
        if (inSignalHandler) {
            PLOGI << "Signal detected, shutting down";
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
```