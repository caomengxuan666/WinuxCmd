# Overview

Boost.Filesystem provides a portable way to perform filesystem operations like path manipulation, directory iteration, and file status queries. It abstracts away platform-specific details (POSIX vs Windows) behind a consistent C++ interface.

**When to use:**
- Cross-platform file and directory operations
- Path manipulation and normalization
- Directory traversal and file metadata queries
- File copying, moving, and deletion

**When NOT to use:**
- For simple string path operations (use `std::filesystem` if C++17 available)
- For high-performance I/O (use OS-specific APIs)
- When you need async filesystem operations (use Boost.Asio)

**Key design:**
- `boost::filesystem::path` class for path representation
- Free functions for operations (e.g., `exists()`, `create_directory()`)
- Exception-based error handling via `filesystem_error`
- Error code overloads for non-throwing operations

```cpp
// Core types
namespace fs = boost::filesystem;
fs::path p("/usr/local/bin");
bool exists = fs::exists(p);
fs::file_status status = fs::status(p);
```