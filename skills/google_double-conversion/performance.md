# Performance

**Performance characteristics**
```cpp
#include <chrono>
#include <vector>

// Benchmark: DoubleToStringConverter vs sprintf
void benchmark_conversion() {
    const int ITERATIONS = 1000000;
    std::vector<double> values = {3.14159, 2.71828, 1.61803, 0.57721};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    char buffer[128];
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    
    for (int i = 0; i < ITERATIONS; ++i) {
        for (double v : values) {
            converter.ToShortest(v, buffer, sizeof(buffer));
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Double-conversion: " << duration.count() << "ms" << std::endl;
}
```

**Allocation patterns**
```cpp
// BAD: Frequent allocations
std::string slow_conversion(double value) {
    char buffer[128];
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    int length = converter.ToShortest(value, buffer, sizeof(buffer));
    return std::string(buffer, length); // Allocation per call
}

// GOOD: Reuse buffer and converter
class FastConverter {
public:
    FastConverter() : converter_(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6) {}
    
    const char* convert(double value, int& length) {
        length = converter_.ToShortest(value, buffer_, sizeof(buffer_));
        return buffer_;
    }
    
private:
    double_conversion::DoubleToStringConverter converter_;
    char buffer_[128];
};
```

**Optimization tips**
```cpp
// Tip 1: Pre-allocate buffers
class BatchProcessor {
public:
    void process(const std::vector<double>& values) {
        results_.clear();
        results_.reserve(values.size());
        
        char buffer[128];
        for (double v : values) {
            int length = converter_.ToShortest(v, buffer, sizeof(buffer));
            results_.emplace_back(buffer, length);
        }
    }
    
private:
    double_conversion::DoubleToStringConverter converter_{
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6};
    std::vector<std::string> results_;
};

// Tip 2: Use ToShortest when possible (fastest)
void use_shortest_when_possible(double value) {
    char buffer[128];
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    
    // ToShortest is faster than ToFixed or ToPrecision
    int length = converter.ToShortest(value, buffer, sizeof(buffer));
    std::cout << std::string(buffer, length) << std::endl;
}
```

**Cache-friendly patterns**
```cpp
// Cache-friendly: Process values in batches
class CacheFriendlyConverter {
public:
    void convert_batch(const double* values, size_t count, char* output_buffer, size_t buffer_size) {
        size_t offset = 0;
        for (size_t i = 0; i < count; ++i) {
            int length = converter_.ToShortest(values[i], 
                output_buffer + offset, buffer_size - offset);
            if (length > 0) {
                output_buffer[offset + length] = '\n'; // Add delimiter
                offset += length + 1;
            }
        }
    }
    
private:
    double_conversion::DoubleToStringConverter converter_{
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6};
};
```