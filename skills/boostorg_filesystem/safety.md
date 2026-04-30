# Safety

**Red Line 1: NEVER pass empty paths to filesystem operations**
```cpp
// BAD: Undefined behavior
fs::path empty;
fs::remove(empty);

// GOOD: Validate paths
fs::path p = get_some_path();
if (!p.empty()) {
    fs::remove(p);
}
```

**Red Line 2: NEVER ignore filesystem_error exceptions in critical code**
```cpp
// BAD: Silently swallows errors
try {
    fs::copy_file("src", "dst");
} catch (...) {}

// GOOD: Log or handle appropriately
try {
    fs::copy_file("src", "dst");
} catch (const fs::filesystem_error& e) {
    std::cerr << "Critical: " << e.what() << "\n";
    throw; // or handle gracefully
}
```

**Red Line 3: NEVER use recursive_directory_iterator without depth limits**
```cpp
// BAD: Can cause stack overflow on deep directories
for (auto& entry : fs::recursive_directory_iterator("/")) {
    // process entire filesystem
}

// GOOD: Limit recursion depth
fs::recursive_directory_iterator it("/", fs::directory_options::skip_permission_denied);
it.no_push(); // Don't recurse by default
for (int i = 0; i < 5 && it != fs::recursive_directory_iterator(); ++it, ++i) {
    // process first 5 levels
}
```

**Red Line 4: NEVER assume path operations are noexcept**
```cpp
// BAD: May throw on invalid paths
fs::path p = "valid" / "path"; // OK
fs::path p2 = "" / "test"; // May throw

// GOOD: Use try-catch or validate
try {
    fs::path p = base_path / relative_path;
} catch (const fs::filesystem_error& e) {
    // Handle invalid path
}
```

**Red Line 5: NEVER mix Boost.Filesystem and C++17 std::filesystem paths**
```cpp
// BAD: Incompatible types
boost::filesystem::path bp = "test";
std::filesystem::path sp = bp; // Compilation error

// GOOD: Convert explicitly
std::filesystem::path sp = bp.string();
// Or use one consistently
```