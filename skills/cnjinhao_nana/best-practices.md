# Best Practices

**Best Practice 1: Use layout managers for resizable UIs**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>

int main() {
    nana::form fm;
    nana::place plc(fm);
    
    nana::label header(fm);
    header.caption("Application Title");
    header.typeface(nana::paint::font("", 14, true)); // Bold
    
    nana::button ok_btn(fm);
    ok_btn.caption("OK");
    
    nana::button cancel_btn(fm);
    cancel_btn.caption("Cancel");
    
    plc.div("vert <header weight=15%><><buttons weight=10% arrange=50>");
    plc["header"] << header;
    plc["buttons"] << ok_btn << cancel_btn;
    plc.collocate();
    
    fm.show();
    nana::exec();
}
```

**Best Practice 2: Handle events with proper lifetime management**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <memory>

class MyApp {
    nana::form fm;
    nana::button btn;
    std::unique_ptr<int> data;
    
public:
    MyApp() : fm(), btn(fm, nana::rectangle(10, 10, 100, 30)) {
        data = std::make_unique<int>(42);
        
        btn.caption("Show Data");
        btn.events().click([this]() {
            nana::msgbox mb(fm, "Data");
            mb << "Value: " << *data;
            mb();
        });
        
        fm.show();
    }
    
    void run() {
        nana::exec();
    }
};

int main() {
    MyApp app;
    app.run();
}
```

**Best Practice 3: Use message boxes for user feedback**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/msgbox.hpp>

int main() {
    nana::form fm;
    nana::button btn(fm, nana::rectangle(10, 10, 150, 30));
    btn.caption("Show Message");
    
    btn.events().click([&fm]() {
        nana::msgbox mb(fm, "Information");
        mb.icon(mb.icon_information);
        mb << "Operation completed successfully!";
        mb();
    });
    
    fm.show();
    nana::exec();
}
```

**Best Practice 4: Organize widgets with clear naming**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>

class LoginForm {
    nana::form fm;
    nana::label username_label;
    nana::textbox username_input;
    nana::label password_label;
    nana::textbox password_input;
    nana::button login_btn;
    
public:
    LoginForm() 
        : fm()
        , username_label(fm, nana::rectangle(10, 10, 80, 25))
        , username_input(fm, nana::rectangle(100, 10, 200, 25))
        , password_label(fm, nana::rectangle(10, 45, 80, 25))
        , password_input(fm, nana::rectangle(100, 45, 200, 25))
        , login_btn(fm, nana::rectangle(100, 80, 100, 30))
    {
        username_label.caption("Username:");
        password_label.caption("Password:");
        password_input.mask_character('*');
        login_btn.caption("Login");
        
        login_btn.events().click([this]() {
            std::string user = username_input.text();
            std::string pass = password_input.text();
            // Validate credentials
        });
        
        fm.caption("Login");
        fm.show();
    }
    
    void run() { nana::exec(); }
};
```

**Best Practice 5: Use timers for periodic updates**
```cpp
#include <nana/gui.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/label.hpp>
#include <chrono>

int main() {
    nana::form fm;
    nana::label clock_label(fm, nana::rectangle(10, 10, 200, 30));
    clock_label.caption("Loading...");
    
    nana::timer update_timer;
    update_timer.elapse([&clock_label]() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::string time_str = std::ctime(&time_t);
        time_str.pop_back(); // Remove newline
        clock_label.caption(time_str);
    });
    update_timer.interval(1000); // Update every second
    update_timer.start();
    
    fm.show();
    nana::exec();
}
```