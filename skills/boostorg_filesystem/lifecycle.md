# Lifecycle

**Construction and destruction of paths**
```cpp
// Default construction
fs::path p1; // empty path

// From string
fs::path p2 = "/usr/local";
fs::path p3 = "C:\\Windows";

// Copy construction
fs::path p4 = p2;

// Move construction
fs::path p5 = std::move(p4);

// Destruction is automatic - no special cleanup needed
```

**Resource management with filesystem objects**
```cpp
// directory_iterator is a resource handle
{
    fs::directory_iterator it("/tmp");
    for (auto& entry : it) {
        // process
    }
} // iterator is closed automatically

// file_status is lightweight, no special management
fs::file_status status = fs::status("/tmp");
```

**Move semantics for paths**
```cpp
fs::path create_temp_path() {
    fs::path p = fs::temp_directory_path() / fs::unique_path();
    return p; // Move or RVO
}

void process_path(fs::path p) {
    // p is moved into function
    fs::create_directories(p);
}
```

**Managing temporary files**
```cpp
class TempFile {
    fs::path path_;
public:
    TempFile() : path_(fs::temp_directory_path() / fs::unique_path()) {}
    ~TempFile() {
        boost::system::error_code ec;
        fs::remove(path_, ec); // Don't throw in destructor
    }
    const fs::path& path() const { return path_; }
};
```