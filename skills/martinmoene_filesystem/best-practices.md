# Best Practices

```cpp
// Best practices for martinmoene/filesystem
#include "filesystem.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace fs = ghc::filesystem;

// 1. Use error_code overloads for non-critical operations
class FileManager {
public:
    bool safe_remove(const fs::path& p) {
        std::error_code ec;
        if (fs::remove(p, ec)) {
            return true;
        }
        if (ec) {
            log_error("Failed to remove " + p.string() + ": " + ec.message());
        }
        return false;
    }
    
    uintmax_t safe_file_size(const fs::path& p) {
        std::error_code ec;
        if (fs::is_regular_file(p, ec)) {
            return fs::file_size(p, ec);
        }
        return 0;
    }

private:
    void log_error(const std::string& msg) {
        std::cerr << msg << "\n";
    }
};

// 2. Create wrapper classes for resource management
class TempDirectory {
public:
    TempDirectory() 
        : path_(fs::temp_directory_path() / unique_name()) {
        fs::create_directory(path_);
    }
    
    ~TempDirectory() {
        std::error_code ec;
        fs::remove_all(path_, ec);
    }
    
    const fs::path& path() const { return path_; }
    
    // Prevent copying
    TempDirectory(const TempDirectory&) = delete;
    TempDirectory& operator=(const TempDirectory&) = delete;
    
    // Allow moving
    TempDirectory(TempDirectory&& other) noexcept 
        : path_(std::move(other.path_)) {
        other.path_.clear();
    }

private:
    fs::path path_;
    
    static std::string unique_name() {
        return "tmp_" + std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count());
    }
};

// 3. Use path composition safely
class PathBuilder {
public:
    PathBuilder& base(const fs::path& base) {
        base_ = fs::absolute(base);
        return *this;
    }
    
    PathBuilder& append(const fs::path& component) {
        components_.push_back(component);
        return *this;
    }
    
    fs::path build() const {
        fs::path result = base_;
        for (auto& c : components_) {
            result /= c;
        }
        return result;
    }

private:
    fs::path base_;
    std::vector<fs::path> components_;
};

// 4. Handle directory iteration efficiently
class DirectoryScanner {
public:
    struct FileInfo {
        fs::path path;
        uintmax_t size;
        fs::file_time_type last_modified;
    };
    
    std::vector<FileInfo> scan_files(const fs::path& dir, 
                                     const std::string& extension = "") {
        std::vector<FileInfo> files;
        std::error_code ec;
        
        if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
            return files;
        }
        
        // Reserve space for efficiency
        files.reserve(1000);
        
        for (auto& entry : fs::directory_iterator(dir, ec)) {
            if (ec) break;
            
            if (fs::is_regular_file(entry.path(), ec)) {
                auto ext = entry.path().extension().string();
                if (extension.empty() || ext == extension) {
                    files.push_back({
                        entry.path(),
                        fs::file_size(entry.path(), ec),
                        fs::last_write_time(entry.path(), ec)
                    });
                }
            }
        }
        
        return files;
    }
};

// 5. Use path normalization
class PathNormalizer {
public:
    static fs::path normalize(const fs::path& p) {
        std::error_code ec;
        fs::path result = fs::absolute(p, ec);
        
        if (!ec) {
            result = result.lexically_normal();
        }
        
        return result;
    }
    
    static bool paths_equal(const fs::path& a, const fs::path& b) {
        return normalize(a) == normalize(b);
    }
};

// 6. Implement retry logic for filesystem operations
class RobustFileOperations {
public:
    static bool copy_with_retry(const fs::path& from, 
                                 const fs::path& to, 
                                 int max_retries = 3) {
        for (int i = 0; i < max_retries; ++i) {
            std::error_code ec;
            if (fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec)) {
                return true;
            }
            
            if (ec.value() != EACCES && ec.value() != EBUSY) {
                // Non-retryable error
                return false;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100 * (i + 1)));
        }
        return false;
    }
};

// 7. Use RAII for file handles
class FileHandle {
public:
    FileHandle(const fs::path& p, std::ios_base::openmode mode = std::ios::out)
        : path_(p), stream_(p.string(), mode) {
        if (!stream_.is_open()) {
            throw std::runtime_error("Failed to open: " + p.string());
        }
    }
    
    ~FileHandle() {
        if (stream_.is_open()) {
            stream_.close();
        }
    }
    
    std::ofstream& stream() { return stream_; }
    const fs::path& path() const { return path_; }

private:
    fs::path path_;
    std::ofstream stream_;
};
```