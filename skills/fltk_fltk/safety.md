# Safety

**RED LINE 1: Never modify widgets from non-main threads without Fl::awake()**
```cpp
// BAD: Direct modification from thread
void worker_thread(Fl_Button* btn) {
    btn->label("Updated"); // CRASH: Not thread-safe
}

// GOOD: Use Fl::awake() for thread-safe updates
void worker_thread(Fl_Button* btn) {
    Fl::awake([](void* data) {
        Fl_Button* b = (Fl_Button*)data;
        b->label("Updated");
    }, btn);
}
```

**RED LINE 2: Never delete widgets directly - use Fl::delete_widget()**
```cpp
// BAD: Direct deletion causes undefined behavior
delete button; // CRASH: May corrupt internal widget tree

// GOOD: Safe deferred deletion
Fl::delete_widget(button); // Safe: Deleted during event loop
```

**RED LINE 3: Never access widgets after their parent window is destroyed**
```cpp
// BAD: Accessing deleted widget
Fl_Window* win = new Fl_Window(400, 300);
Fl_Button* btn = new Fl_Button(10, 10, 100, 30, "Click");
win->end();
delete win;
btn->label("New"); // CRASH: btn is deleted with window

// GOOD: Track widget lifecycle
Fl_Window* win = new Fl_Window(400, 300);
Fl_Button* btn = new Fl_Button(10, 10, 100, 30, "Click");
win->end();
// Only use btn while win is alive
```

**RED LINE 4: Never call Fl::run() more than once**
```cpp
// BAD: Multiple event loops
Fl::run(); // First loop
Fl::run(); // CRASH: Second loop is undefined

// GOOD: Single event loop
return Fl::run(); // Only call once
```

**RED LINE 5: Never use widgets before calling show() on their parent window**
```cpp
// BAD: Widget operations before window is shown
Fl_Window* win = new Fl_Window(400, 300);
Fl_Button* btn = new Fl_Button(10, 10, 100, 30, "Click");
win->end();
btn->position(20, 20); // May not work correctly before show()
win->show();

// GOOD: Set properties after window is shown
Fl_Window* win = new Fl_Window(400, 300);
Fl_Button* btn = new Fl_Button(10, 10, 100, 30, "Click");
win->end();
win->show();
btn->position(20, 20); // Safe after window is shown
```