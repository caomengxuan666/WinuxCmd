# Threading

**Thread safety with Fl::awake()**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <thread>
#include <atomic>

std::atomic<bool> running{true};

void worker_thread(Fl_Button* btn) {
    int count = 0;
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Safe: Use Fl::awake() to update GUI from thread
        Fl::awake([btn, count](void*) {
            char buf[32];
            snprintf(buf, sizeof(buf), "Count: %d", count);
            btn->label(buf);
        }, nullptr);
        count++;
    }
}

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Button* button = new Fl_Button(10, 10, 380, 280, "Waiting...");
    window->end();
    
    std::thread worker(worker_thread, button);
    
    window->show();
    int result = Fl::run();
    
    running = false;
    worker.join();
    return result;
}
```

**Using Fl::lock() for thread synchronization**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <thread>

void thread_safe_update(Fl_Button* btn, const char* text) {
    Fl::lock(); // Acquire FLTK lock
    btn->label(text);
    btn->redraw();
    Fl::unlock(); // Release FLTK lock
    Fl::awake(); // Wake up main thread
}

int main() {
    Fl::lock(); // Initialize threading support
    
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Button* button = new Fl_Button(10, 10, 380, 280, "Thread Safe");
    window->end();
    
    std::thread worker(thread_safe_update, button, "Updated from thread");
    
    window->show();
    int result = Fl::run();
    
    worker.join();
    return result;
}
```

**Thread-safe message passing**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Output.H>
#include <queue>
#include <mutex>
#include <thread>

std::queue<std::string> message_queue;
std::mutex queue_mutex;

void producer_thread() {
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        std::lock_guard<std::mutex> lock(queue_mutex);
        message_queue.push("Message " + std::to_string(i));
        
        Fl::awake(); // Signal main thread to process messages
    }
}

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Output* output = new Fl_Output(10, 10, 380, 280, "Messages:");
    window->end();
    
    // Add idle callback to process messages
    Fl::add_idle([](void* data) {
        Fl_Output* out = (Fl_Output*)data;
        std::lock_guard<std::mutex> lock(queue_mutex);
        
        while (!message_queue.empty()) {
            out->value(message_queue.front().c_str());
            message_queue.pop();
        }
    }, output);
    
    std::thread producer(producer_thread);
    
    window->show();
    int result = Fl::run();
    
    producer.join();
    return result;
}
```

**Thread-safe timer implementation**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <thread>
#include <atomic>

class SafeTimer {
    Fl_Box* display;
    std::atomic<bool> active{true};
    std::thread worker;
    
public:
    SafeTimer(Fl_Box* d) : display(d) {
        worker = std::thread([this]() {
            int count = 0;
            while (active) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                
                // Thread-safe update using Fl::awake()
                Fl::awake([this, count](void*) {
                    if (active) {
                        char buf[32];
                        snprintf(buf, sizeof(buf), "Timer: %d", count);
                        display->label(buf);
                        display->redraw();
                    }
                }, nullptr);
                count++;
            }
        });
    }
    
    ~SafeTimer() {
        active = false;
        if (worker.joinable()) {
            worker.join();
        }
    }
};

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Box* box = new Fl_Box(10, 10, 380, 280, "Timer Demo");
    box->box(FL_UP_BOX);
    box->labelsize(24);
    window->end();
    
    SafeTimer timer(box);
    
    window->show();
    return Fl::run();
}
```