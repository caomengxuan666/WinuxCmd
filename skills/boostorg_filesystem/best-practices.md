# Best Practices

**Use error_code overloads for non-critical operations**
```cpp
boost::system::error_code ec;
fs::remove("temp.txt", ec);
if (ec) {
    // Log but don't throw
}
```

**Prefer canonical paths for comparison**
```cpp
fs::path p1 = fs::canonical("/usr/bin/../bin/gcc");
fs::path p2 = fs::canonical("/usr/bin/gcc");
if (p1 == p2) {
    // Same file
}
```

**Use directory_options for permission handling**
```cpp
fs::recursive_directory_iterator it(
    "/var/log",
    fs::directory_options::skip_permission_denied |
    fs::directory_options::follow_directory_symlink
);
```

**Implement proper cleanup with RAII**
```cpp
class TempDirectory {
    fs::path dir_;
public:
    TempDirectory() : dir_(fs::temp_directory_path() / fs::unique_path()) {
        fs::create_directories(dir_);
    }
    ~TempDirectory() {
        fs::remove_all(dir_);
    }
    const fs::path& path() const { return dir_; }
};
```

**Use path decomposition for safe path manipulation**
```cpp
fs::path p = "/home/user/docs/report.pdf";
std::cout << "Parent: " << p.parent_path() << "\n";
std::cout << "Stem: " << p.stem() << "\n";
std::cout << "Extension: " << p.extension() << "\n";
```