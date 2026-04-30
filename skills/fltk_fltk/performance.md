# Performance

**Minimize widget creation overhead**
```cpp
// BAD: Creating many individual widgets
for (int i = 0; i < 1000; i++) {
    new Fl_Button(x, y, w, h, label); // High overhead
}

// GOOD: Use Fl_Group to batch widget creation
Fl_Group* group = new Fl_Group(0, 0, 400, 300);
for (int i = 0; i < 1000; i++) {
    new Fl_Button(x, y, w, h, label); // More efficient in group
}
group->end();
```

**Optimize redraw operations**
```cpp
// BAD: Triggering unnecessary redraws
void update_widget(Fl_Widget* w) {
    w->redraw(); // Triggers full redraw
    w->label("New");
    w->redraw(); // Second redraw - wasteful
}

// GOOD: Batch changes and redraw once
void update_widget(Fl_Widget* w) {
    w->label("New");
    w->redraw(); // Single redraw after all changes
}
```

**Use Fl::check() for non-blocking event processing**
```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    window->show();
    
    // Non-blocking event processing for animations
    while (window->shown()) {
        Fl::check(); // Process events without blocking
        // Update animation state here
        Fl::redraw();
    }
    
    return 0;
}
```

**Memory allocation patterns**
```cpp
// BAD: Frequent allocation/deallocation
void update_display() {
    static Fl_Window* win = nullptr;
    if (win) delete win;
    win = new Fl_Window(400, 300); // Frequent allocation
    win->show();
}

// GOOD: Reuse existing widgets
Fl_Window* win = new Fl_Window(400, 300);
Fl_Box* box = new Fl_Box(10, 10, 380, 280, "Initial");
win->end();

void update_display(const char* new_text) {
    box->label(new_text); // Reuse existing widget
    box->redraw();
}
```