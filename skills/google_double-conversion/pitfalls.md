# Pitfalls

**Pitfall 1: Buffer overflow**
```cpp
// BAD: Buffer too small for output
char buffer[10];
double_conversion::DoubleToStringConverter converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);
converter.ToShortest(123456789.123456789, buffer, sizeof(buffer)); // Overflow!

// GOOD: Use adequate buffer size
char buffer[128];
double_conversion::DoubleToStringConverter converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);
int length = converter.ToShortest(123456789.123456789, buffer, sizeof(buffer));
if (length > 0 && length < sizeof(buffer)) {
    std::cout << std::string(buffer, length) << std::endl;
}
```

**Pitfall 2: Ignoring processed_characters**
```cpp
// BAD: Not checking how many characters were processed
double_conversion::StringToDoubleConverter converter(
    double_conversion::StringToDoubleConverter::NO_FLAGS,
    0.0, 0.0, "inf", "nan");
double result = converter.StringToDouble("123abc", 6, nullptr); // Returns 123.0 but ignores trailing junk

// GOOD: Check processed characters
int processed = 0;
double result = converter.StringToDouble("123abc", 6, &processed);
if (processed != 6) {
    std::cout << "Warning: Only processed " << processed << " characters" << std::endl;
}
```

**Pitfall 3: Wrong flags for special values**
```cpp
// BAD: Not allowing NaN/infinity strings
double_conversion::StringToDoubleConverter converter(
    double_conversion::StringToDoubleConverter::NO_FLAGS,
    0.0, 0.0, "inf", "nan");
int processed = 0;
double result = converter.StringToDouble("inf", 3, &processed); // Returns 0.0, not infinity

// GOOD: Allow special values
double_conversion::StringToDoubleConverter converter(
    double_conversion::StringToDoubleConverter::ALLOW_SPECIAL_FLOATS,
    0.0, 0.0, "inf", "nan");
int processed = 0;
double result = converter.StringToDouble("inf", 3, &processed); // Returns infinity
```

**Pitfall 4: Assuming thread safety**
```cpp
// BAD: Sharing converter across threads without synchronization
double_conversion::DoubleToStringConverter global_converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);

void thread_func() {
    char buffer[128];
    global_converter.ToShortest(3.14, buffer, sizeof(buffer)); // Not thread-safe!
}

// GOOD: Create local converter per thread
void thread_func() {
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    char buffer[128];
    converter.ToShortest(3.14, buffer, sizeof(buffer));
}
```

**Pitfall 5: Incorrect buffer size for ToFixed**
```cpp
// BAD: Not accounting for decimal point and sign
char buffer[10];
double_conversion::DoubleToStringConverter converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);
converter.ToFixed(-1234.5678, 4, buffer, sizeof(buffer)); // Needs more space

// GOOD: Calculate required buffer size
int required = 20; // Sign + digits + decimal point + fraction + null
char buffer[64];
double_conversion::DoubleToStringConverter converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);
converter.ToFixed(-1234.5678, 4, buffer, sizeof(buffer));
```

**Pitfall 6: Not handling negative zero**
```cpp
// BAD: Negative zero gets formatted as "-0.0"
double negative_zero = -0.0;
char buffer[128];
double_conversion::DoubleToStringConverter converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);
converter.ToShortest(negative_zero, buffer, sizeof(buffer)); // "-0.0"

// GOOD: Use UNIQUE_ZERO flag to format as "0.0"
double_conversion::DoubleToStringConverter converter(
    double_conversion::DoubleToStringConverter::UNIQUE_ZERO,
    "inf", "nan", 'e', 6, 6);
converter.ToShortest(negative_zero, buffer, sizeof(buffer)); // "0.0"
```