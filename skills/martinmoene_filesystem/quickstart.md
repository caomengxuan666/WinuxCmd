# Quickstart

```cpp
// Quickstart examples for martinmoene/filesystem
#include "filesystem.hpp"
#include <iostream>
#include <string>

namespace fs = ghc::filesystem;

// 1. Path construction and manipulation
void path_examples() {
    fs::path p1 = "/home/user/docs/file.txt";
    fs::path p2 = "relative/path";
    
    std::cout << "Filename: " << p1.filename() << "\n";
    std::cout << "Extension: " << p1.extension() << "\n";
    std::cout << "Parent path: " << p1.parent_path() << "\n";
    std::cout << "Root: " << p1.root_name() << "\n";
}

// 2. File existence and status
void file_status_examples() {
    fs::path p = "test.txt";
    
    if (fs::exists(p)) {
        std::cout << "File exists\n";
        auto size = fs::file_size(p);
        auto time = fs::last_write_time(p);
        std::cout << "Size: " << size << " bytes\n";
    }
}

// 3. Directory iteration
void directory_iteration() {
    fs::path dir = ".";
    
    for (auto& entry : fs::directory_iterator(dir)) {
        std::cout << entry.path() << "\n";
    }
    
    // Recursive iteration
    for (auto& entry : fs::recursive_directory_iterator(dir)) {
        if (fs::is_regular_file(entry.path())) {
            std::cout << "File: " << entry.path() << "\n";
        }
    }
}

// 4. File creation and removal
void file_operations() {
    fs::path dir = "new_directory";
    fs::path file = dir / "test.txt";
    
    // Create directory
    fs::create_directory(dir);
    
    // Create file
    std::ofstream(file.string()) << "Hello\n";
    
    // Copy file
    fs::copy(file, dir / "copy.txt");
    
    // Remove
    fs::remove(file);
    fs::remove_all(dir);
}

// 5. Path decomposition
void path_decomposition() {
    fs::path p = "/home/user/docs/file.txt";
    
    std::cout << "Root path: " << p.root_path() << "\n";
    std::cout << "Relative path: " << p.relative_path() << "\n";
    std::cout << "Stem: " << p.stem() << "\n";
    
    // Iterate over components
    for (auto& part : p) {
        std::cout << "Part: " << part << "\n";
    }
}

// 6. File permissions
void permissions_example() {
    fs::path p = "script.sh";
    
    // Check permissions
    auto perms = fs::status(p).permissions();
    
    if ((perms & fs::perms::owner_exec) != fs::perms::none) {
        std::cout << "File is executable\n";
    }
    
    // Set permissions
    fs::permissions(p, fs::perms::owner_all | fs::perms::group_read);
}

// 7. Space information
void space_info_example() {
    fs::path p = "/";
    
    auto space = fs::space(p);
    std::cout << "Capacity: " << space.capacity << " bytes\n";
    std::cout << "Free: " << space.free << " bytes\n";
    std::cout << "Available: " << space.available << " bytes\n";
}

// 8. Temporary files
void temp_file_example() {
    fs::path temp_dir = fs::temp_directory_path();
    fs::path temp_file = temp_dir / "temp_XXXXXX.txt";
    
    // Create unique temporary file
    std::string template_str = temp_file.string();
    int fd = mkstemp(&template_str[0]);
    if (fd != -1) {
        close(fd);
        std::cout << "Created temp file: " << template_str << "\n";
        fs::remove(template_str);
    }
}
```