# Lifecycle

**Construction**
```cpp
// Default construction with explicit flags
double_conversion::DoubleToStringConverter converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "infinity",        // Infinity string
    "not_a_number",    // NaN string
    'e',               // Exponent separator
    6,                 // Decimal in exponent (max)
    6);                // Decimal in precision (max)

// StringToDoubleConverter construction
double_conversion::StringToDoubleConverter str_converter(
    double_conversion::StringToDoubleConverter::ALLOW_SPECIAL_FLOATS |
    double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK,
    0.0,               // Default value for NaN
    0.0,               // Default value for infinity
    "inf",             // Infinity string
    "nan");            // NaN string
```

**Destruction**
```cpp
// Converters are trivially destructible - no special cleanup needed
{
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    // Use converter...
} // Automatically destroyed, no resources to release
```

**Move semantics**
```cpp
// Converters support move semantics
class ConverterWrapper {
public:
    ConverterWrapper() : converter_(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6) {}
    
    // Move constructor
    ConverterWrapper(ConverterWrapper&& other) noexcept 
        : converter_(std::move(other.converter_)) {}
    
    // Move assignment
    ConverterWrapper& operator=(ConverterWrapper&& other) noexcept {
        if (this != &other) {
            converter_ = std::move(other.converter_);
        }
        return *this;
    }
    
    double_conversion::DoubleToStringConverter& get() { return converter_; }
    
private:
    double_conversion::DoubleToStringConverter converter_;
};
```

**Resource management**
```cpp
// Converters are lightweight - no heap allocation
class ConversionCache {
public:
    ConversionCache() : converter_(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6) {}
    
    // Copy is cheap - just copies flags and strings
    ConversionCache(const ConversionCache& other) = default;
    ConversionCache& operator=(const ConversionCache& other) = default;
    
    std::string convert(double value) {
        char buffer[128];
        int length = converter_.ToShortest(value, buffer, sizeof(buffer));
        return std::string(buffer, length);
    }
    
private:
    double_conversion::DoubleToStringConverter converter_;
};
```