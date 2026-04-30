# Safety

```cpp
// Safety conditions for martinmoene/filesystem
#include "filesystem.hpp"
#include <iostream>
#include <system_error>

namespace fs = ghc::filesystem;

// CONDITION 1: NEVER ignore error codes from filesystem operations
void bad_ignore_errors() {
    fs::path p = "/invalid/path";
    // BAD: Unchecked operation can throw
    fs::remove(p);  // Throws filesystem_error
}

void good_handle_errors() {
    fs::path p = "/invalid/path";
    std::error_code ec;
    
    // GOOD: Always check error codes
    if (fs::remove(p, ec)) {
        // Success
    } else if (ec) {
        std::cerr << "Error: " << ec.message() << "\n";
    }
}

// CONDITION 2: NEVER assume paths exist without checking
void bad_assume_existence() {
    fs::path p = "config.txt";
    // BAD: May throw if file doesn't exist
    auto time = fs::last_write_time(p);
}

void good_check_existence() {
    fs::path p = "config.txt";
    std::error_code ec;
    
    // GOOD: Check existence and handle errors
    if (fs::exists(p, ec)) {
        auto time = fs::last_write_time(p, ec);
        if (!ec) {
            std::cout << "Last modified: " << time << "\n";
        }
    }
}

// CONDITION 3: NEVER use file_size() on non-regular files
void bad_file_size_on_directory() {
    fs::path dir = ".";
    // BAD: Will throw on directory
    auto size = fs::file_size(dir);
}

void good_file_size_check() {
    fs::path dir = ".";
    std::error_code ec;
    
    // GOOD: Check file type first
    if (fs::is_regular_file(dir, ec)) {
        auto size = fs::file_size(dir, ec);
        std::cout << "Size: " << size << "\n";
    } else {
        std::cout << "Not a regular file\n";
    }
}

// CONDITION 4: NEVER modify filesystem while iterating
void bad_modify_while_iterating() {
    fs::path dir = ".";
    // BAD: Removing files during iteration is undefined behavior
    for (auto it = fs::directory_iterator(dir); it != fs::directory_iterator(); ++it) {
        fs::remove(it->path());  // Undefined behavior!
    }
}

void good_collect_then_modify() {
    fs::path dir = ".";
    std::vector<fs::path> to_remove;
    
    // GOOD: Collect paths first
    for (auto& entry : fs::directory_iterator(dir)) {
        to_remove.push_back(entry.path());
    }
    
    // Then modify
    for (auto& p : to_remove) {
        std::error_code ec;
        fs::remove(p, ec);
    }
}

// CONDITION 5: NEVER use relative paths without understanding current directory
void bad_relative_paths() {
    // BAD: Depends on current working directory
    fs::path p = "data.txt";
    std::ofstream(p.string()) << "data";
    
    // Later, if CWD changes, this fails
    fs::current_path("/tmp");
    if (fs::exists(p)) {  // Now checks /tmp/data.txt
        // Unexpected behavior
    }
}

void good_absolute_paths() {
    // GOOD: Use absolute paths or document assumptions
    fs::path base_dir = fs::absolute(".");
    fs::path p = base_dir / "data.txt";
    
    std::ofstream(p.string()) << "data";
    
    // Or use a configuration-based approach
    fs::path config_dir = fs::path(getenv("CONFIG_DIR")) / "data.txt";
    if (fs::exists(config_dir)) {
        // Safe to use
    }
}
```