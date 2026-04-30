# Lifecycle

```cpp
// Lifecycle management for martinmoene/filesystem
#include "filesystem.hpp"
#include <iostream>
#include <memory>

namespace fs = ghc::filesystem;

// 1. Path construction and destruction
void path_lifecycle() {
    // Default construction
    fs::path p1;  // Empty path
    
    // String construction
    fs::path p2 = "/home/user/file.txt";
    fs::path p3 = "relative/path";
    
    // Copy construction
    fs::path p4 = p2;
    
    // Move construction
    fs::path p5 = std::move(p4);
    // p4 is now in valid but unspecified state
    
    // Assignment
    p1 = p5;
    
    // Destruction happens automatically at end of scope
}

// 2. Resource management with RAII
class FileResource {
public:
    FileResource(const fs::path& p) 
        : path_(p) {
        // Create parent directories if needed
        std::error_code ec;
        fs::create_directories(path_.parent_path(), ec);
        
        // Create the file
        std::ofstream(path_.string()) << "Initial content\n";
    }
    
    ~FileResource() {
        // Clean up in destructor
        std::error_code ec;
        if (fs::exists(path_, ec)) {
            fs::remove(path_, ec);
        }
    }
    
    // Move semantics
    FileResource(FileResource&& other) noexcept 
        : path_(std::move(other.path_)) {
        other.path_.clear();  // Leave moved-from in valid state
    }
    
    FileResource& operator=(FileResource&& other) noexcept {
        if (this != &other) {
            // Clean up current resource
            std::error_code ec;
            if (fs::exists(path_, ec)) {
                fs::remove(path_, ec);
            }
            
            // Take ownership
            path_ = std::move(other.path_);
            other.path_.clear();
        }
        return *this;
    }
    
    // Delete copy operations
    FileResource(const FileResource&) = delete;
    FileResource& operator=(const FileResource&) = delete;
    
    const fs::path& path() const { return path_; }

private:
    fs::path path_;
};

// 3. Directory lifecycle management
class DirectoryManager {
public:
    DirectoryManager(const fs::path& base) 
        : base_path_(fs::absolute(base)) {
        std::error_code ec;
        fs::create_directories(base_path_, ec);
        if (ec) {
            throw std::runtime_error("Failed to create directory: " + ec.message());
        }
    }
    
    ~DirectoryManager() {
        // Optional: clean up on destruction
        cleanup();
    }
    
    // Move constructor
    DirectoryManager(DirectoryManager&& other) noexcept 
        : base_path_(std::move(other.base_path_)) {
        other.base_path_.clear();
    }
    
    // Move assignment
    DirectoryManager& operator=(DirectoryManager&& other) noexcept {
        if (this != &other) {
            cleanup();
            base_path_ = std::move(other.base_path_);
            other.base_path_.clear();
        }
        return *this;
    }
    
    fs::path create_subdirectory(const std::string& name) {
        fs::path sub = base_path_ / name;
        std::error_code ec;
        fs::create_directory(sub, ec);
        if (ec) {
            throw std::runtime_error("Failed to create subdirectory: " + ec.message());
        }
        return sub;
    }
    
    void cleanup() {
        if (!base_path_.empty()) {
            std::error_code ec;
            fs::remove_all(base_path_, ec);
        }
    }

private:
    fs::path base_path_;
};

// 4. Temporary file management
class TempFile {
public:
    TempFile() {
        // Create unique temporary file
        auto temp_dir = fs::temp_directory_path();
        path_ = temp_dir / "tmp_XXXXXX";
        
        std::string template_str = path_.string();
        int fd = mkstemp(&template_str[0]);
        if (fd == -1) {
            throw std::runtime_error("Failed to create temp file");
        }
        close(fd);
        path_ = template_str;
    }
    
    ~TempFile() {
        std::error_code ec;
        fs::remove(path_, ec);
    }
    
    // Move semantics
    TempFile(TempFile&& other) noexcept 
        : path_(std::move(other.path_)) {
        other.path_.clear();
    }
    
    TempFile& operator=(TempFile&& other) noexcept {
        if (this != &other) {
            std::error_code ec;
            fs::remove(path_, ec);
            path_ = std::move(other.path_);
            other.path_.clear();
        }
        return *this;
    }
    
    const fs::path& path() const { return path_; }

private:
    fs::path path_;
};

// 5. Path ownership and lifetime
class PathOwner {
public:
    // Takes ownership of a path
    explicit PathOwner(fs::path p) 
        : path_(std::move(p)) {}
    
    // Shares a path reference
    const fs::path& get() const { return path_; }
    
    // Releases ownership
    fs::path release() {
        fs::path released = std::move(path_);
        path_.clear();
        return released;
    }

private:
    fs::path path_;
};

// 6. Example of proper lifecycle management
void lifecycle_example() {
    // Create a temporary directory
    TempDirectory temp_dir;
    std::cout << "Working in: " << temp_dir.path() << "\n";
    
    // Create files in it
    {
        FileResource file1(temp_dir.path() / "file1.txt");
        FileResource file2(temp_dir.path() / "file2.txt");
        
        // Move file
        FileResource file3 = std::move(file1);
        // file1 is now empty/invalid
        
        // Use files...
    }  // Files are cleaned up here
    
    // Directory is cleaned up when temp_dir goes out of scope
}
```