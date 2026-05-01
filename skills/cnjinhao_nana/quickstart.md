# Quickstart

```cpp
// 1. Basic window creation
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>

int main()
{
    nana::form fm;
    fm.caption("Hello Nana");
    
    nana::label lbl(fm, nana::rectangle(10, 10, 200, 30));
    lbl.caption("Welcome to Nana GUI!");
    
    nana::button btn(fm, nana::rectangle(10, 50, 100, 30));
    btn.caption("Click Me");
    
    btn.events().click([&]() {
        lbl.caption("Button clicked!");
    });
    
    fm.show();
    nana::exec();
}
```

```cpp
// 2. Event handling with lambda
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

int main()
{
    nana::form fm;
    nana::button btn(fm, nana::rectangle(10, 10, 150, 30));
    btn.caption("Exit");
    
    btn.events().click([&fm]() {
        fm.close();
    });
    
    fm.show();
    nana::exec();
}
```

```cpp
// 3. Using layout manager
#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>

int main()
{
    nana::form fm;
    nana::place plc(fm);
    
    nana::label lbl(fm);
    lbl.caption("Resizable label");
    
    nana::button btn(fm);
    btn.caption("OK");
    
    plc.div("vert <label weight=80%><button weight=20%>");
    plc["label"] << lbl;
    plc["button"] << btn;
    plc.collocate();
    
    fm.show();
    nana::exec();
}
```

```cpp
// 4. Drawing on a form
#include <nana/gui.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/paint/graphics.hpp>

int main()
{
    nana::form fm;
    fm.caption("Drawing Example");
    
    nana::drawing dw(fm);
    dw.draw([](nana::paint::graphics& graph) {
        graph.rectangle(nana::rectangle(50, 50, 100, 100), true, nana::colors::red);
        graph.line(nana::point(50, 50), nana::point(150, 150), nana::colors::blue);
    });
    
    fm.show();
    nana::exec();
}
```

```cpp
// 5. Timer usage
#include <nana/gui.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/label.hpp>

int main()
{
    nana::form fm;
    nana::label lbl(fm, nana::rectangle(10, 10, 200, 30));
    lbl.caption("Timer not started");
    
    nana::timer timer;
    timer.elapse([&lbl]() {
        static int count = 0;
        lbl.caption("Tick: " + std::to_string(++count));
    });
    timer.interval(1000);
    timer.start();
    
    fm.show();
    nana::exec();
}
```

```cpp
// 6. Listbox with items
#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>

int main()
{
    nana::form fm;
    nana::listbox lb(fm, nana::rectangle(10, 10, 300, 200));
    lb.append_header("Name", 100);
    lb.append_header("Age", 50);
    
    lb.at(0).append({"Alice", "25"});
    lb.at(0).append({"Bob", "30"});
    lb.at(0).append({"Charlie", "35"});
    
    lb.events().selected([&](const nana::arg_listbox& arg) {
        auto item = arg.item;
        if (!item.empty()) {
            nana::msgbox mb(fm, "Selection");
            mb << "Selected: " << item.text(0);
            mb();
        }
    });
    
    fm.show();
    nana::exec();
}
```

```cpp
// 7. Menu bar
#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>

int main()
{
    nana::form fm;
    nana::menubar mb(fm);
    
    auto& file = mb.push_back("File");
    file.append("Open", [&]() {
        nana::msgbox mb(fm, "Menu");
        mb << "Open clicked";
        mb();
    });
    file.append("Exit", [&fm]() {
        fm.close();
    });
    
    auto& edit = mb.push_back("Edit");
    edit.append("Copy", []() {});
    edit.append("Paste", []() {});
    
    fm.show();
    nana::exec();
}
```

```cpp
// 8. File dialog
#include <nana/gui.hpp>
#include <nana/gui/filebox.hpp>

int main()
{
    nana::form fm;
    
    nana::filebox fbox(fm, true); // true = open file
    fbox.add_filter("Text Files", "*.txt");
    fbox.add_filter("All Files", "*.*");
    
    if (fbox.show()) {
        auto files = fbox.result();
        if (!files.empty()) {
            nana::msgbox mb(fm, "Selected File");
            mb << files[0];
            mb();
        }
    }
    
    fm.show();
    nana::exec();
}
```