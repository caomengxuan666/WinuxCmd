# Lifecycle

### Construction
```cpp
// Create App with description
CLI::App app{"My application description"};

// Create with custom name for help
CLI::App app2{"My app", "my_app"};

// Create subcommand
auto* sub = app.add_subcommand("sub", "Subcommand description");
```

### Destruction
```cpp
// Apps are automatically cleaned up when they go out of scope
{
    CLI::App app;
    // ... use app ...
} // app destroyed, all options cleaned up

// Subcommands are owned by parent, no manual deletion needed
auto* sub = app.add_subcommand("cmd", "Command");
// sub will be destroyed when app is destroyed
```

### Move semantics
```cpp
// CLI::App supports move construction
CLI::App createApp() {
    CLI::App app{"Temporary"};
    app.add_option("--value", value);
    return app;  // Move is efficient
}

CLI::App app = createApp();  // Move constructor

// Move assignment
CLI::App app1{"First"}, app2{"Second"};
app1 = std::move(app2);  // app1 now has app2's options
```

### Resource management
```cpp
// Options are owned by the App
CLI::App app;
auto* opt = app.add_option("--name", name);
// opt is valid as long as app exists

// Don't delete option pointers manually
// delete opt;  // WRONG - app owns it

// To remove an option, use remove_option
app.remove_option(opt);  // Safe removal
```