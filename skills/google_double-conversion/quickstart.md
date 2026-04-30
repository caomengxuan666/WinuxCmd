# Quickstart

```cpp
#include <double-conversion/double-conversion.h>
#include <iostream>
#include <string>

// Pattern 1: Double to string with default settings
void double_to_string_basic() {
    double value = 3.14159;
    char buffer[128];
    
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    
    int length = converter.ToShortest(value, buffer, sizeof(buffer));
    std::cout << std::string(buffer, length) << std::endl; // "3.14159"
}

// Pattern 2: Double to string with fixed precision
void double_to_string_fixed() {
    double value = 123.456789;
    char buffer[128];
    
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    
    int length = converter.ToFixed(value, 2, buffer, sizeof(buffer));
    std::cout << std::string(buffer, length) << std::endl; // "123.46"
}

// Pattern 3: String to double conversion
void string_to_double() {
    const char* input = "3.14159";
    int processed_characters = 0;
    double result;
    
    double_conversion::StringToDoubleConverter converter(
        double_conversion::StringToDoubleConverter::NO_FLAGS,
        0.0, 0.0, "inf", "nan");
    
    result = converter.StringToDouble(input, strlen(input), &processed_characters);
    std::cout << result << std::endl; // 3.14159
}

// Pattern 4: Handling special values
void special_values() {
    double inf = std::numeric_limits<double>::infinity();
    double nan = std::numeric_limits<double>::quiet_NaN();
    char buffer[128];
    
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "infinity", "not_a_number", 'e', 6, 6);
    
    int length = converter.ToShortest(inf, buffer, sizeof(buffer));
    std::cout << std::string(buffer, length) << std::endl; // "infinity"
}

// Pattern 5: Using flags for custom formatting
void custom_formatting() {
    double value = 1.0 / 3.0;
    char buffer[128];
    
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::UNIQUE_ZERO |
        double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN,
        "inf", "nan", 'e', 6, 6);
    
    int length = converter.ToShortest(value, buffer, sizeof(buffer));
    std::cout << std::string(buffer, length) << std::endl;
}

// Pattern 6: Error handling in string to double
void error_handling() {
    const char* invalid = "not_a_number";
    int processed_characters = 0;
    double result;
    
    double_conversion::StringToDoubleConverter converter(
        double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK,
        0.0, 0.0, "inf", "nan");
    
    result = converter.StringToDouble(invalid, strlen(invalid), &processed_characters);
    if (processed_characters == 0) {
        std::cout << "Conversion failed" << std::endl;
    }
}
```