# Safety

**Safety Condition 1: NEVER use invalidated listbox iterators**
```cpp
// BAD: Using iterator after erase
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

void unsafe_erase(nana::listbox& lb) {
    auto it = lb.at(0).begin();
    lb.at(0).erase(it);
    auto next = it; // CRASH - undefined behavior
    next++;
}

// GOOD: Use index-based operations
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

void safe_erase(nana::listbox& lb) {
    auto count = lb.at(0).size();
    if (count > 0) {
        lb.at(0).erase(0); // Safe: erase by index
    }
}
```

**Safety Condition 2: NEVER access widgets after form destruction**
```cpp
// BAD: Using widget after form destroyed
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

void unsafe_widget() {
    nana::form* fm = new nana::form();
    nana::button* btn = new nana::button(*fm, nana::rectangle(10, 10, 100, 30));
    delete fm; // Form destroyed
    btn->caption("Click"); // CRASH - button's parent is gone
    delete btn;
}

// GOOD: Destroy widgets before their parent
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

void safe_widget() {
    nana::form fm;
    nana::button btn(fm, nana::rectangle(10, 10, 100, 30));
    btn.caption("Click");
    // btn destroyed before fm when going out of scope
}
```

**Safety Condition 3: NEVER call `unsort()` after sorting listbox**
```cpp
// BAD: unsort causes selection mismatch
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

void unsafe_unsort(nana::listbox& lb) {
    lb.sort(0, true);
    lb.unsort(); // Items now have wrong selection mapping
}

// GOOD: Maintain sorted state or re-sort
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

void safe_sort(nana::listbox& lb) {
    lb.sort(0, true);
    // Keep sorted, or re-sort with different criteria
    lb.sort(0, false); // Re-sort descending instead
}
```

**Safety Condition 4: NEVER ignore DPI scaling differences**
```cpp
// BAD: Menu text renders incorrectly across monitors
#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>

void unsafe_menu() {
    nana::form fm;
    nana::menubar mb(fm);
    auto& file = mb.push_back("File");
    file.append("Open", []() {});
    // Text may be cut off or misaligned on different DPI
}

// GOOD: Handle DPI changes explicitly
#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>

void safe_menu() {
    nana::form fm;
    nana::menubar mb(fm);
    
    fm.events().resized([&]() {
        mb.move(nana::rectangle(0, 0, fm.size().width, 30));
    });
    
    auto& file = mb.push_back("File");
    file.append("Open", []() {});
}
```

**Safety Condition 5: NEVER use `std::round` without proper includes**
```cpp
// BAD: Missing include for std::round
#include <nana/gui.hpp>

void unsafe_round() {
    double val = 3.7;
    int rounded = std::round(val); // May fail to compile with some compilers
}

// GOOD: Include <cmath> explicitly
#include <nana/gui.hpp>
#include <cmath>

void safe_round() {
    double val = 3.7;
    int rounded = static_cast<int>(std::round(val));
}
```