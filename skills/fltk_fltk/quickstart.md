# Quickstart

```cpp
// Basic FLTK application skeleton
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

int main() {
    Fl_Window* window = new Fl_Window(340, 180);
    Fl_Button* button = new Fl_Button(20, 40, 300, 100, "Click Me!");
    button->callback([](Fl_Widget* w, void*) {
        w->label("Clicked!");
    });
    window->end();
    window->show();
    return Fl::run();
}
```

```cpp
// Creating a simple form with input fields
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>

int main() {
    Fl_Window* window = new Fl_Window(400, 300, "Login Form");
    
    Fl_Input* username = new Fl_Input(100, 50, 200, 25, "Username:");
    Fl_Input* password = new Fl_Input(100, 100, 200, 25, "Password:");
    password->type(FL_SECRET_INPUT);
    
    Fl_Button* login = new Fl_Button(150, 200, 100, 30, "Login");
    login->callback([](Fl_Widget*, void* data) {
        Fl_Input** inputs = (Fl_Input**)data;
        printf("Username: %s, Password: %s\n", inputs[0]->value(), inputs[1]->value());
    }, (void*)new Fl_Input*[2]{username, password});
    
    window->end();
    window->show();
    return Fl::run();
}
```

```cpp
// Using Fl_Box for static text
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

int main() {
    Fl_Window* window = new Fl_Window(300, 200);
    Fl_Box* box = new Fl_Box(20, 40, 260, 100, "Hello, FLTK!");
    box->box(FL_UP_BOX);
    box->labelfont(FL_BOLD);
    box->labelsize(36);
    box->labeltype(FL_SHADOW_LABEL);
    window->end();
    window->show();
    return Fl::run();
}
```

```cpp
// Handling multiple widgets with callbacks
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>

int main() {
    Fl_Window* window = new Fl_Window(400, 200);
    Fl_Output* output = new Fl_Output(100, 50, 200, 25, "Result:");
    
    Fl_Button* btn1 = new Fl_Button(50, 100, 100, 30, "Add");
    btn1->callback([](Fl_Widget*, void* data) {
        Fl_Output* out = (Fl_Output*)data;
        out->value("Button 1 clicked");
    }, output);
    
    Fl_Button* btn2 = new Fl_Button(200, 100, 100, 30, "Clear");
    btn2->callback([](Fl_Widget*, void* data) {
        Fl_Output* out = (Fl_Output*)data;
        out->value("");
    }, output);
    
    window->end();
    window->show();
    return Fl::run();
}
```

```cpp
// Creating a menu bar
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>

int main() {
    Fl_Window* window = new Fl_Window(400, 300);
    Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, 400, 25);
    menu->add("File/Open", FL_CTRL + 'o', [](Fl_Widget*, void*) {
        printf("Open selected\n");
    });
    menu->add("File/Save", FL_CTRL + 's', [](Fl_Widget*, void*) {
        printf("Save selected\n");
    });
    menu->add("Edit/Copy", FL_CTRL + 'c', [](Fl_Widget*, void*) {
        printf("Copy selected\n");
    });
    menu->add("Edit/Paste", FL_CTRL + 'v', [](Fl_Widget*, void*) {
        printf("Paste selected\n");
    });
    window->end();
    window->show();
    return Fl::run();
}
```

```cpp
// Using Fl_Choice for dropdown menus
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>

int main() {
    Fl_Window* window = new Fl_Window(300, 200);
    Fl_Choice* choice = new Fl_Choice(100, 50, 150, 25, "Select:");
    choice->add("Option 1");
    choice->add("Option 2");
    choice->add("Option 3");
    choice->value(0); // Select first option
    
    choice->callback([](Fl_Widget* w, void*) {
        Fl_Choice* c = (Fl_Choice*)w;
        printf("Selected: %s\n", c->text());
    });
    
    window->end();
    window->show();
    return Fl::run();
}
```