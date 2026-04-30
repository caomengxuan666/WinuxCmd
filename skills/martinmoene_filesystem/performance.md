# Performance

```cpp
// Performance characteristics of martinmoene/filesystem
#include "filesystem.hpp"
#include <iostream>
#include <chrono>
#include <vector>

namespace fs = ghc::filesystem;

// 1. Path construction overhead
void path_construction_performance() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // BAD: Repeated path construction
    for (int i = 0; i < 10000; ++i) {
        fs::path p = "/home/user/documents/file_" + std::to_string(i) + ".txt";
        // Use p
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Repeated construction: " << duration.count() << "ms\n";
    
    // GOOD: Reuse path objects
    start = std::chrono::high_resolution_clock::now();
    
    fs::path base = "/home/user/documents";
    for (int i = 0; i < 10000; ++i) {
        fs::path p = base / ("file_" + std::to_string(i) + ".txt");
        // Use p
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Reused base path: " << duration.count() << "ms\n";
}

// 2. Directory iteration performance
void directory_iteration_performance() {
    fs::path dir = "/usr/include";
    
    // BAD: Multiple iterations
    auto start = std::chrono::high_resolution_clock::now();
    
    // First pass: count files
    int count = 0;
    for (auto& entry : fs::recursive_directory_iterator(dir)) {
        ++count;
    }
    
    // Second pass: collect names
    std::vector<fs::path> names;
    names.reserve(count);
    for (auto& entry : fs::recursive_directory_iterator(dir)) {
        names.push_back(entry.path());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Two-pass iteration: " << duration.count() << "ms\n";
    
    // GOOD: Single pass
    start = std::chrono::high_resolution_clock::now();
    
    std::vector<fs::path> names2;
    names2.reserve(10000);  // Pre-allocate
    for (auto& entry : fs::recursive_directory_iterator(dir)) {
        names2.push_back(entry.path());
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Single-pass iteration: " << duration.count() << "ms\n";
}

// 3. File status caching
void file_status_caching() {
    fs::path p = "test.txt";
    
    // BAD: Multiple status calls
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        if (fs::exists(p)) {
            auto size = fs::file_size(p);
            auto time = fs::last_write_time(p);
            // Use info
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Multiple status calls: " << duration.count() << "ms\n";
    
    // GOOD: Cache status
    start = std::chrono::high_resolution_clock::now();
    
    std::error_code ec;
    auto status = fs::status(p, ec);
    if (fs::exists(status)) {
        auto size = fs::file_size(p, ec);
        auto time = fs::last_write_time(p, ec);
        // Use cached info
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Cached status: " << duration.count() << "ms\n";
}

// 4. String conversion overhead
void string_conversion_performance() {
    fs::path p = "/home/user/documents/file.txt";
    
    // BAD: Repeated string conversion
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        std::string s = p.string();
        // Use s
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Repeated string conversion: " << duration.count() << "ms\n";
    
    // GOOD: Convert once
    start = std::chrono::high_resolution_clock::now();
    
    std::string s = p.string();
    for (int i = 0; i < 10000; ++i) {
        // Use s directly
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Single conversion: " << duration.count() << "ms\n";
}

// 5. Batch operations optimization
void batch_operations_performance() {
    fs::path dir = "test_dir";
    std::vector<fs::path> files;
    
    // Create test files
    fs::create_directory(dir);
    for (int i = 0; i < 1000; ++i) {
        fs::path p = dir / ("file_" + std::to_string(i) + ".txt");
        std::ofstream(p.string()) << "content";
        files.push_back(p);
    }
    
    // BAD: Individual operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (auto& p : files) {
        std::error_code ec;
        fs::remove(p, ec);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Individual removes: " << duration.count() << "ms\n";
    
    // GOOD: Batch remove
    // Recreate files
    for (int i = 0; i < 1000; ++i) {
        fs::path p = dir / ("file_" + std::to_string(i) + ".txt");
        std::ofstream(p.string()) << "content";
    }
    
    start = std::chrono::high_resolution_clock::now();
    
    std::error_code ec;
    fs::remove_all(dir, ec);  // Single operation
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Batch remove: " << duration.count() << "ms\n";
}

// 6. Memory allocation patterns
void memory_allocation_patterns() {
    // BAD: Frequent allocations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        fs::path p = fs::temp_directory_path() / ("file_" + std::to_string(i) + ".txt");
        // Use p
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Frequent allocations: " << duration.count() << "ms\n";
    
    // GOOD: Cache and reuse
    start = std::chrono::high_resolution_clock::now();
    
    fs::path temp_dir = fs::temp_directory_path();
    for (int i = 0; i < 10000; ++i) {
        fs::path p = temp_dir / ("file_" + std::to_string(i) + ".txt");
        // Use p
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Cached temp dir: " << duration.count() << "ms\n";
}
```