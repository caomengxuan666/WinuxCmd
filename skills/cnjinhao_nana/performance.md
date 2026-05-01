# Performance

**Performance Characteristic 1: Widget creation overhead**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <vector>

// BAD: Creating many widgets individually
void create_many_widgets_bad(nana::form& fm) {
    std::vector<nana::button*> btns;
    for (int i = 0; i < 1000; ++i) {
        btns.push_back(new nana::button(
            fm, nana::rectangle(10, i * 25, 100, 20)));
    }
    // Each creation has overhead
}

// GOOD: Reuse widgets or create only what's needed
void create_few_widgets_good(nana::form& fm) {
    nana::button btn(fm, nana::rectangle(10, 10, 100, 30));
    btn.caption("Dynamic Content");
    // Update content instead of creating new widgets
}
```

**Performance Characteristic 2: Drawing operations**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/paint/graphics.hpp>

// BAD: Redrawing entire surface unnecessarily
void bad_drawing(nana::drawing& dw) {
    dw.draw([](nana::paint::graphics& graph) {
        graph.rectangle(nana::rectangle(0, 0, 800, 600), 
                       true, nana::colors::white);
        // Expensive full redraw
    });
}

// GOOD: Only redraw changed regions
void good_drawing(nana::drawing& dw) {
    dw.draw([](nana::paint::graphics& graph) {
        // Only draw what changed
        graph.rectangle(nana::rectangle(10, 10, 100, 100), 
                       false, nana::colors::red);
    });
}
```

**Performance Characteristic 3: Timer intervals**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/timer.hpp>

// BAD: Very short timer intervals cause high CPU usage
void bad_timer() {
    nana::timer timer;
    timer.elapse([]() {
        // Expensive operation every millisecond
    });
    timer.interval(1); // 1ms interval - CPU intensive
    timer.start();
}

// GOOD: Use appropriate intervals
void good_timer() {
    nana::timer timer;
    timer.elapse([]() {
        // Lightweight update
    });
    timer.interval(100); // 100ms - reasonable for UI updates
    timer.start();
}
```

**Performance Characteristic 4: Listbox with many items**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <string>

// BAD: Adding items one by one
void bad_listbox_populate(nana::listbox& lb) {
    for (int i = 0; i < 10000; ++i) {
        lb.at(0).append({std::to_string(i)});
    }
    // Each append triggers UI update
}

// GOOD: Batch updates when possible
void good_listbox_populate(nana::listbox& lb) {
    lb.auto_draw(false); // Disable automatic redraw
    for (int i = 0; i < 10000; ++i) {
        lb.at(0).append({std::to_string(i)});
    }
    lb.auto_draw(true); // Re-enable and redraw once
}
```