# Quickstart

```cpp
#include <boost/filesystem.hpp>
#include <iostream>

namespace fs = boost::filesystem;

// Pattern 1: Check if a path exists
void check_path() {
    fs::path p("/tmp/test.txt");
    if (fs::exists(p)) {
        std::cout << "File exists\n";
    }
}

// Pattern 2: Create a directory
void create_directory() {
    fs::path dir("/tmp/new_dir");
    if (!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}

// Pattern 3: Iterate over directory contents
void list_directory() {
    fs::path dir(".");
    for (auto& entry : fs::directory_iterator(dir)) {
        std::cout << entry.path() << "\n";
    }
}

// Pattern 4: Get file size
void file_size_example() {
    fs::path p("example.txt");
    if (fs::exists(p) && fs::is_regular_file(p)) {
        std::uintmax_t size = fs::file_size(p);
        std::cout << "Size: " << size << " bytes\n";
    }
}

// Pattern 5: Copy a file
void copy_file_example() {
    fs::path src("source.txt");
    fs::path dst("destination.txt");
    try {
        fs::copy_file(src, dst, fs::copy_option::overwrite_if_exists);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

// Pattern 6: Get current working directory
void current_path_example() {
    fs::path cwd = fs::current_path();
    std::cout << "Current directory: " << cwd << "\n";
}

// Pattern 7: Remove a file
void remove_file() {
    fs::path p("temp.txt");
    if (fs::exists(p)) {
        fs::remove(p);
    }
}

// Pattern 8: Create a symbolic link (POSIX)
void create_symlink() {
    fs::path target("/usr/bin/gcc");
    fs::path link("gcc_link");
    if (!fs::exists(link)) {
        fs::create_symlink(target, link);
    }
}
```