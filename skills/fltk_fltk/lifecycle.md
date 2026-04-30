# Lifecycle

**Widget construction and parent-child relationships**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

int main() {
    // Construction: Widgets are automatically parented to current group
    Fl_Window* window = new Fl_Window(400, 300, "Lifecycle Demo");
    
    // These buttons are automatically children of 'window'
    Fl_Button* btn1 = new Fl_Button(10, 10, 100, 30, "Button 1");
    Fl_Button* btn2 = new Fl_Button(10, 50, 100, 30, "Button 2");
    
    window->end(); // Stops automatic parenting
    window->show();
    return Fl::run();
}
```

**Proper destruction sequence**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

class MyApp {
    Fl_Window* window;
    Fl_Button* button;
public:
    MyApp() {
        window = new Fl_Window(400, 300);
        button = new Fl_Button(10, 10, 100, 30, "Click");
        window->end();
        
        // Set up safe cleanup
        window->callback([](Fl_Widget* w, void*) {
            Fl::delete_widget(w); // Deletes window and all children
        });
    }
    
    void run() {
        window->show();
        Fl::run();
    }
    
    ~MyApp() {
        // No need to manually delete - Fl::delete_widget handles it
    }
};
```

**Resource management with Fl::delete_widget()**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Button* button = new Fl_Button(10, 10, 100, 30, "Delete Me");
    
    // Safe self-deletion
    button->callback([](Fl_Widget* w, void*) {
        Fl::delete_widget(w); // Safe: Deleted during next event loop iteration
    });
    
    window->end();
    window->show();
    return Fl::run();
}
```

**Move semantics and widget ownership**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <memory>

// FLTK doesn't support move semantics natively
// Use smart pointers for safe ownership management
class WidgetManager {
    std::unique_ptr<Fl_Window> window;
    Fl_Button* button; // Raw pointer - owned by window
public:
    WidgetManager() {
        window = std::make_unique<Fl_Window>(400, 300);
        button = new Fl_Button(10, 10, 100, 30, "Managed");
        window->end();
    }
    
    void show() { window->show(); }
    
    // Move constructor transfers ownership
    WidgetManager(WidgetManager&& other) noexcept 
        : window(std::move(other.window)), 
          button(other.button) {
        other.button = nullptr;
    }
};
```