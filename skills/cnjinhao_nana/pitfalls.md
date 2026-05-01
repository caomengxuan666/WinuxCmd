# Pitfalls

**Pitfall 1: Forgetting to call `nana::exec()`**
```cpp
// BAD: Window never appears
#include <nana/gui.hpp>

int main() {
    nana::form fm;
    fm.show();
    // Missing nana::exec() - program exits immediately
    return 0;
}

// GOOD: Proper event loop
#include <nana/gui.hpp>

int main() {
    nana::form fm;
    fm.show();
    nana::exec(); // Starts the event loop
    return 0;
}
```

**Pitfall 2: Using invalidated iterators after listbox erase**
```cpp
// BAD: Iterator invalidated after erase
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

void bad_erase(nana::listbox& lb) {
    auto it = lb.at(0).begin();
    lb.at(0).erase(it);
    // it is now invalid - using it causes undefined behavior
    auto next = it; // CRASH!
}

// GOOD: Use index-based approach
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

void good_erase(nana::listbox& lb) {
    auto count = lb.at(0).size();
    if (count > 0) {
        lb.at(0).erase(0); // Erase by index
    }
}
```

**Pitfall 3: DPI scaling issues with menu drawing**
```cpp
// BAD: Menu text renders incorrectly on different DPI monitors
#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>

int main() {
    nana::form fm;
    nana::menubar mb(fm);
    auto& file = mb.push_back("File");
    file.append("Open", []() {});
    // Text may be misaligned when moving between DPI scales
    fm.show();
    nana::exec();
}

// GOOD: Explicitly handle DPI changes
#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>

int main() {
    nana::form fm;
    nana::menubar mb(fm);
    
    fm.events().resized([&]() {
        // Recalculate layout on resize
        mb.move(nana::rectangle(0, 0, fm.size().width, 30));
    });
    
    auto& file = mb.push_back("File");
    file.append("Open", []() {});
    fm.show();
    nana::exec();
}
```

**Pitfall 4: Not setting widget parent correctly**
```cpp
// BAD: Widget created without parent
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

int main() {
    nana::button btn; // No parent - will not display
    btn.caption("Click");
    nana::form fm;
    fm.show();
    nana::exec();
}

// GOOD: Always specify parent form
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

int main() {
    nana::form fm;
    nana::button btn(fm, nana::rectangle(10, 10, 100, 30));
    btn.caption("Click");
    fm.show();
    nana::exec();
}
```

**Pitfall 5: Using `unsort` after sorting listbox**
```cpp
// BAD: unsort causes item selection mismatch
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

int main() {
    nana::form fm;
    nana::listbox lb(fm, nana::rectangle(10, 10, 300, 200));
    lb.append_header("Name", 100);
    lb.at(0).append({"Charlie"});
    lb.at(0).append({"Alice"});
    lb.at(0).append({"Bob"});
    
    lb.sort(0, true); // Sort ascending
    lb.unsort(); // Now clicking item 2 selects wrong item
    
    fm.show();
    nana::exec();
}

// GOOD: Avoid unsort, or re-sort instead
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

int main() {
    nana::form fm;
    nana::listbox lb(fm, nana::rectangle(10, 10, 300, 200));
    lb.append_header("Name", 100);
    lb.at(0).append({"Charlie"});
    lb.at(0).append({"Alice"});
    lb.at(0).append({"Bob"});
    
    // Instead of unsort, maintain sorted order
    lb.sort(0, true);
    
    fm.show();
    nana::exec();
}
```

**Pitfall 6: Compilation errors with newer compilers**
```cpp
// BAD: Using deprecated or removed features
#include <nana/gui.hpp>
#include <nana/gui/widgets/textbox.hpp>

int main() {
    nana::form fm;
    nana::textbox tb(fm, nana::rectangle(10, 10, 200, 100));
    // May fail with GCC 14.1 or VS2022 C++20
    tb.caption("Hello");
    fm.show();
    nana::exec();
}

// GOOD: Use current API
#include <nana/gui.hpp>
#include <nana/gui/widgets/textbox.hpp>

int main() {
    nana::form fm;
    nana::textbox tb(fm, nana::rectangle(10, 10, 200, 100));
    tb.append("Hello", false); // Use append instead of caption
    fm.show();
    nana::exec();
}
```