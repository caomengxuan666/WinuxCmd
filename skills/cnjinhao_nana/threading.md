# Threading

**Thread Safety: Nana is NOT thread-safe for most operations**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <thread>

// BAD: Modifying UI from worker thread
void bad_threading() {
    nana::form fm;
    nana::label lbl(fm, nana::rectangle(10, 10, 200, 30));
    
    std::thread worker([&lbl]() {
        lbl.caption("Updated from thread"); // CRASH - not thread-safe
    });
    
    fm.show();
    nana::exec();
    worker.join();
}

// GOOD: Use post to update UI from threads
void good_threading() {
    nana::form fm;
    nana::label lbl(fm, nana::rectangle(10, 10, 200, 30));
    
    std::thread worker([&fm, &lbl]() {
        // Do work in background
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Post update to UI thread
        nana::internal_scope_guard lock;
        lbl.caption("Updated safely");
    });
    
    fm.show();
    nana::exec();
    worker.join();
}
```

**Thread Safety: Use `nana::internal_scope_guard` for thread-safe updates**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <thread>
#include <atomic>

class ThreadSafeApp {
    nana::form fm;
    nana::button btn;
    std::atomic<int> counter{0};
    std::thread worker;
    
public:
    ThreadSafeApp() 
        : fm()
        , btn(fm, nana::rectangle(10, 10, 150, 30))
    {
        btn.caption("Start Worker");
        
        btn.events().click([this]() {
            worker = std::thread([this]() {
                for (int i = 0; i < 10; ++i) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    counter++;
                    
                    // Thread-safe UI update
                    nana::internal_scope_guard lock;
                    btn.caption("Count: " + std::to_string(counter));
                }
            });
            worker.detach();
        });
        
        fm.show();
    }
    
    ~ThreadSafeApp() {
        if (worker.joinable()) {
            worker.join();
        }
    }
};
```

**Thread Safety: Use timers instead of threads for periodic UI updates**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/label.hpp>
#include <atomic>

// BAD: Using threads for UI updates
void bad_ui_update() {
    nana::form fm;
    nana::label lbl(fm, nana::rectangle(10, 10, 200, 30));
    
    std::thread updater([&lbl]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            nana::internal_scope_guard lock;
            lbl.caption("Update");
        }
    });
    updater.detach();
    
    fm.show();
    nana::exec();
}

// GOOD: Use timer for UI updates
void good_ui_update() {
    nana::form fm;
    nana::label lbl(fm, nana::rectangle(10, 10, 200, 30));
    
    nana::timer timer;
    timer.elapse([&lbl]() {
        static int count = 0;
        lbl.caption("Count: " + std::to_string(++count));
    });
    timer.interval(1000);
    timer.start();
    
    fm.show();
    nana::exec();
}
```

**Thread Safety: Protect shared data with mutexes**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <thread>
#include <mutex>

class DataManager {
    nana::listbox& lb;
    std::mutex data_mutex;
    std::vector<std::string> shared_data;
    
public:
    DataManager(nana::listbox& list) : lb(list) {}
    
    void add_from_thread(const std::string& data) {
        std::lock_guard<std::mutex> lock(data_mutex);
        shared_data.push_back(data);
        
        // Thread-safe UI update
        nana::internal_scope_guard ui_lock;
        lb.at(0).append({data});
    }
    
    void process_in_background() {
        std::thread worker([this]() {
            for (int i = 0; i < 100; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                add_from_thread("Item " + std::to_string(i));
            }
        });
        worker.detach();
    }
};
```