# Pitfalls

**BAD: Forgetting to call end() on window**
```cpp
Fl_Window* window = new Fl_Window(400, 300);
Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
// Missing window->end() - button won't be properly parented
window->show();
return Fl::run();
```

**GOOD: Properly ending window construction**
```cpp
Fl_Window* window = new Fl_Window(400, 300);
Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
window->end(); // Important: ends widget addition
window->show();
return Fl::run();
```

**BAD: Using widgets after window is destroyed**
```cpp
Fl_Window* window = new Fl_Window(400, 300);
Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
window->end();
window->show();
delete window; // Destroys all child widgets
button->label("New label"); // Undefined behavior - button is deleted
```

**GOOD: Proper widget lifecycle management**
```cpp
Fl_Window* window = new Fl_Window(400, 300);
Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
window->end();
window->show();
// Don't access button after window is closed/destroyed
// Or use Fl::delete_widget() for safe deletion
```

**BAD: Modifying widgets from non-main thread**
```cpp
#include <thread>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

void update_button(Fl_Button* btn) {
    btn->label("Updated"); // BAD: Not thread-safe
}

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
    window->end();
    window->show();
    
    std::thread t(update_button, button);
    t.detach();
    
    return Fl::run();
}
```

**GOOD: Using Fl::awake() for thread-safe updates**
```cpp
#include <thread>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

void update_button(Fl_Button* btn) {
    Fl::awake([](void* data) {
        Fl_Button* b = (Fl_Button*)data;
        b->label("Updated");
    }, btn);
}

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
    window->end();
    window->show();
    
    std::thread t(update_button, button);
    t.detach();
    
    return Fl::run();
}
```

**BAD: Memory leak with dynamically allocated widgets**
```cpp
void create_window() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
    window->end();
    window->show();
    // window is never deleted - memory leak
}
```

**GOOD: Proper cleanup with Fl::delete_widget()**
```cpp
void create_window() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Click");
    window->end();
    window->show();
    // Schedule safe deletion when window is closed
    window->callback([](Fl_Widget* w, void*) {
        Fl::delete_widget(w);
    });
}
```