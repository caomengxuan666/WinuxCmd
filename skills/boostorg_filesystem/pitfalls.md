# Pitfalls

**Pitfall 1: Not checking existence before operations**
```cpp
// BAD: May throw if file doesn't exist
fs::remove("nonexistent.txt");

// GOOD: Check first
if (fs::exists("nonexistent.txt")) {
    fs::remove("nonexistent.txt");
}
```

**Pitfall 2: Ignoring filesystem_error exceptions**
```cpp
// BAD: Unhandled exception
fs::create_directory("/root/test");

// GOOD: Handle with try-catch
try {
    fs::create_directory("/root/test");
} catch (const fs::filesystem_error& e) {
    std::cerr << "Failed: " << e.what() << "\n";
}
```

**Pitfall 3: Using relative paths without context**
```cpp
// BAD: Depends on current working directory
fs::remove("data.txt");

// GOOD: Use absolute path
fs::path data_path = fs::current_path() / "data.txt";
fs::remove(data_path);
```

**Pitfall 4: Not handling symlinks correctly**
```cpp
// BAD: Follows symlinks, may cause infinite loops
for (auto& entry : fs::recursive_directory_iterator(".")) {
    // process
}

// GOOD: Skip symlinks or use canonical paths
for (auto& entry : fs::recursive_directory_iterator(".")) {
    if (fs::is_symlink(entry.path())) continue;
    // process
}
```

**Pitfall 5: Assuming path separators are forward slashes**
```cpp
// BAD: Hardcoded separator
fs::path p = "C:\\Users\\test";

// GOOD: Use generic format or native format
fs::path p = "C:/Users/test"; // generic format works everywhere
```

**Pitfall 6: Not checking file type before operations**
```cpp
// BAD: May fail on directories
std::uintmax_t size = fs::file_size("/tmp");

// GOOD: Check type first
if (fs::is_regular_file("/tmp")) {
    std::uintmax_t size = fs::file_size("/tmp");
}
```

**Pitfall 7: Race conditions with exists() + operation**
```cpp
// BAD: TOCTOU race condition
if (fs::exists("file.txt")) {
    fs::remove("file.txt"); // File may be deleted between check and operation
}

// GOOD: Just attempt the operation and handle errors
try {
    fs::remove("file.txt");
} catch (const fs::filesystem_error&) {
    // File didn't exist or couldn't be removed
}
```