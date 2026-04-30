# Performance

**Path operations are generally fast but can be expensive**
```cpp
// Fast: string manipulation only
fs::path p = "/usr/local/bin";
fs::path parent = p.parent_path(); // O(1) or O(n)

// Slow: requires system calls
bool exists = fs::exists(p); // System call
fs::file_status status = fs::status(p); // System call
```

**Directory iteration performance**
```cpp
// Pre-allocate if you know the size
std::vector<fs::path> entries;
entries.reserve(1000); // If you expect ~1000 entries
for (auto& entry : fs::directory_iterator("/tmp")) {
    entries.push_back(entry.path());
}

// Avoid recursive_directory_iterator for large trees
// Use iterative approach instead
```

**Minimize system calls**
```cpp
// BAD: Multiple system calls
if (fs::exists(p) && fs::is_regular_file(p)) {
    auto size = fs::file_size(p);
}

// GOOD: Single system call
auto status = fs::status(p);
if (fs::is_regular_file(status)) {
    auto size = fs::file_size(p); // Still one more call
}
```

**Use canonical() sparingly**
```cpp
// Expensive: resolves all symlinks
fs::path canonical = fs::canonical(p);

// Cheaper: just normalize the path string
fs::path normalized = p.lexically_normal();
```