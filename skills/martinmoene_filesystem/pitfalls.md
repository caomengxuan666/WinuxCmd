# Pitfalls

```cpp
// Common pitfalls with martinmoene/filesystem
#include "filesystem.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace fs = ghc::filesystem;

// PITFALL 1: Not checking existence before operations
void bad_check_existence() {
    fs::path p = "nonexistent.txt";
    // BAD: Will throw if file doesn't exist
    auto size = fs::file_size(p);  // Throws filesystem_error
}

void good_check_existence() {
    fs::path p = "nonexistent.txt";
    std::error_code ec;
    
    // GOOD: Check existence first
    if (fs::exists(p, ec)) {
        auto size = fs::file_size(p, ec);
        if (!ec) {
            std::cout << "Size: " << size << "\n";
        }
    }
}

// PITFALL 2: Ignoring error codes
void bad_ignore_errors() {
    fs::path p = "/root/protected_file.txt";
    // BAD: Will throw on permission error
    fs::remove(p);  // Throws if no permission
}

void good_handle_errors() {
    fs::path p = "/root/protected_file.txt";
    std::error_code ec;
    
    // GOOD: Handle errors gracefully
    if (fs::remove(p, ec)) {
        std::cout << "Removed successfully\n";
    } else {
        std::cerr << "Failed to remove: " << ec.message() << "\n";
    }
}

// PITFALL 3: Path separator assumptions
void bad_path_separators() {
    // BAD: Hardcoded separators
    fs::path p = "dir1\\dir2\\file.txt";  // Fails on Linux
    std::cout << p << "\n";
}

void good_path_separators() {
    // GOOD: Use operator/ or forward slashes
    fs::path p = "dir1/dir2/file.txt";  // Works everywhere
    p = fs::path("dir1") / "dir2" / "file.txt";
    std::cout << p << "\n";
}

// PITFALL 4: Directory iterator invalidation
void bad_iterator_invalidation() {
    fs::path dir = ".";
    // BAD: Modifying directory while iterating
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".tmp") {
            fs::remove(entry.path());  // May invalidate iterator
        }
    }
}

void good_iterator_invalidation() {
    fs::path dir = ".";
    std::vector<fs::path> to_remove;
    
    // GOOD: Collect paths first, then remove
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".tmp") {
            to_remove.push_back(entry.path());
        }
    }
    
    for (auto& p : to_remove) {
        fs::remove(p);
    }
}

// PITFALL 5: Recursive directory iteration depth
void bad_recursive_depth() {
    fs::path dir = "/";
    // BAD: Could cause stack overflow on deep directories
    for (auto& entry : fs::recursive_directory_iterator(dir)) {
        // Process all files
    }
}

void good_recursive_depth() {
    fs::path dir = "/";
    // GOOD: Limit recursion depth
    fs::recursive_directory_iterator it(dir, fs::directory_options::skip_permission_denied);
    it.disable_recursion_pending();  // Start with no recursion
    
    int max_depth = 5;
    for (; it != fs::recursive_directory_iterator(); ++it) {
        if (it.depth() >= max_depth) {
            it.disable_recursion_pending();  // Don't go deeper
        }
        // Process entry
    }
}

// PITFALL 6: Symlink handling
void bad_symlink_handling() {
    fs::path link = "link_to_dir";
    // BAD: May follow symlinks unexpectedly
    if (fs::is_directory(link)) {
        // This follows symlinks
    }
}

void good_symlink_handling() {
    fs::path link = "link_to_dir";
    std::error_code ec;
    
    // GOOD: Check if it's a symlink first
    if (fs::is_symlink(link, ec)) {
        fs::path target = fs::read_symlink(link, ec);
        std::cout << "Symlink to: " << target << "\n";
    }
    
    // Use symlink_options for directory iteration
    for (auto& entry : fs::directory_iterator(link, 
         fs::directory_options::follow_directory_symlink)) {
        // Explicitly control symlink following
    }
}

// PITFALL 7: File size for directories
void bad_file_size() {
    fs::path dir = ".";
    // BAD: file_size() on directory throws
    auto size = fs::file_size(dir);  // Throws!
}

void good_file_size() {
    fs::path dir = ".";
    std::error_code ec;
    
    // GOOD: Check if it's a regular file first
    if (fs::is_regular_file(dir, ec)) {
        auto size = fs::file_size(dir, ec);
        std::cout << "Size: " << size << "\n";
    } else {
        // Calculate directory size manually
        uintmax_t total_size = 0;
        for (auto& entry : fs::recursive_directory_iterator(dir, ec)) {
            if (fs::is_regular_file(entry.path(), ec)) {
                total_size += fs::file_size(entry.path(), ec);
            }
        }
        std::cout << "Directory size: " << total_size << "\n";
    }
}

// PITFALL 8: Unicode path handling
void bad_unicode_paths() {
    // BAD: Assuming ASCII paths
    fs::path p = "résumé.txt";
    std::ofstream(p.string()) << "Content";  // May fail on some systems
}

void good_unicode_paths() {
    // GOOD: Use native encoding
    fs::path p = u8"résumé.txt";
    
    // Use u8string() for UTF-8 paths
    std::ofstream(p.u8string()) << "Content";
    
    // Or use wstring for Windows
    #ifdef _WIN32
        std::ofstream(p.wstring()) << "Content";
    #else
        std::ofstream(p.string()) << "Content";
    #endif
}
```