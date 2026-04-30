# Best Practices

**Use Fl::delete_widget() for safe widget cleanup**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

class MyWindow : public Fl_Window {
public:
    MyWindow() : Fl_Window(400, 300) {
        Fl_Button* btn = new Fl_Button(10, 10, 100, 30, "Close");
        btn->callback([](Fl_Widget* w, void* data) {
            Fl_Window* win = (Fl_Window*)data;
            Fl::delete_widget(win); // Safe cleanup
        }, this);
        end();
    }
};
```

**Implement proper callback management**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <memory>

class SafeCallback {
    std::shared_ptr<bool> alive;
public:
    SafeCallback() : alive(std::make_shared<bool>(true)) {}
    
    void makeCallback(Fl_Button* btn) {
        auto alivePtr = alive;
        btn->callback([](Fl_Widget* w, void* data) {
            auto alive = *static_cast<std::shared_ptr<bool>*>(data);
            if (*alive) {
                // Safe to access widget
                w->label("Clicked");
            }
        }, &alive);
    }
    
    void invalidate() { *alive = false; }
};
```

**Use Fl_Group for organized layouts**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    
    // Create a group for related controls
    Fl_Group* controlGroup = new Fl_Group(10, 10, 380, 100, "Controls");
    controlGroup->box(FL_ENGRAVED_BOX);
    
    new Fl_Button(20, 30, 100, 30, "Button 1");
    new Fl_Button(140, 30, 100, 30, "Button 2");
    new Fl_Button(260, 30, 100, 30, "Button 3");
    
    controlGroup->end();
    window->end();
    window->show();
    return Fl::run();
}
```

**Handle window close events properly**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

int main() {
    Fl_Window* window = new Fl_Window(400, 300, "My App");
    
    // Custom close handler
    window->callback([](Fl_Widget* w, void*) {
        int result = fl_choice("Are you sure you want to quit?", 
                               "Cancel", "Quit", nullptr);
        if (result == 1) {
            Fl::delete_widget(w);
        }
    });
    
    window->show();
    return Fl::run();
}
```