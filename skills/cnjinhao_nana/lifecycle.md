# Lifecycle

**Construction: Widgets require a parent form**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

int main() {
    // Correct construction with parent
    nana::form fm;
    nana::button btn(fm, nana::rectangle(10, 10, 100, 30));
    
    // Alternative: construct then move
    nana::button btn2;
    btn2.create(fm, nana::rectangle(10, 50, 100, 30));
    
    fm.show();
    nana::exec();
}
```

**Destruction: Widgets are destroyed before their parent**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

class MyWindow {
    nana::form fm;
    nana::button btn; // Declared after form
    
public:
    MyWindow() : fm(), btn(fm, nana::rectangle(10, 10, 100, 30)) {
        btn.caption("Click");
        fm.show();
    }
    // btn destroyed first, then fm (reverse declaration order)
};

int main() {
    MyWindow win;
    nana::exec();
}
```

**Move semantics: Widgets can be moved but not copied**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <vector>

int main() {
    nana::form fm;
    
    // Move button into vector
    std::vector<nana::button> buttons;
    buttons.emplace_back(fm, nana::rectangle(10, 10, 100, 30));
    buttons.emplace_back(fm, nana::rectangle(10, 50, 100, 30));
    
    buttons[0].caption("Button 1");
    buttons[1].caption("Button 2");
    
    // Move assignment
    nana::button btn3;
    btn3 = std::move(buttons[0]); // btn3 now owns the widget
    
    fm.show();
    nana::exec();
}
```

**Resource management: Use RAII for automatic cleanup**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <memory>

class ResourceManager {
    nana::form fm;
    std::unique_ptr<nana::label> status_label;
    std::unique_ptr<nana::timer> refresh_timer;
    
public:
    ResourceManager() : fm() {
        status_label = std::make_unique<nana::label>(
            fm, nana::rectangle(10, 10, 200, 30));
        status_label->caption("Ready");
        
        refresh_timer = std::make_unique<nana::timer>();
        refresh_timer->elapse([this]() {
            status_label->caption("Refreshing...");
        });
        refresh_timer->interval(5000);
        refresh_timer->start();
        
        fm.show();
    }
    // All resources automatically cleaned up
};

int main() {
    ResourceManager mgr;
    nana::exec();
}
```