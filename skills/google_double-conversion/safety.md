# Safety

**Red-line 1: Buffer overflow must NEVER occur**
```cpp
// BAD: No bounds checking
void unsafe_convert(double value, char* buffer, int size) {
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    converter.ToShortest(value, buffer, size); // May overflow
}

// GOOD: Always check return value
bool safe_convert(double value, char* buffer, int size) {
    double_conversion::DoubleToStringConverter converter(
        double_conversion::DoubleToStringConverter::NO_FLAGS,
        "inf", "nan", 'e', 6, 6);
    int length = converter.ToShortest(value, buffer, size);
    return length > 0 && length < size;
}
```

**Red-line 2: Uninitialized processed_characters must NEVER be used**
```cpp
// BAD: Using uninitialized processed count
int processed;
double result = converter.StringToDouble("123.45", 6, &processed);
if (processed > 0) { /* processed may be garbage */ }

// GOOD: Initialize before use
int processed = 0;
double result = converter.StringToDouble("123.45", 6, &processed);
if (processed > 0) { /* Safe to use */ }
```

**Red-line 3: String length must NEVER be incorrect**
```cpp
// BAD: Wrong string length
const char* str = "3.14159";
double result = converter.StringToDouble(str, 3, &processed); // Only reads "3.1"

// GOOD: Use correct length
double result = converter.StringToDouble(str, strlen(str), &processed);
```

**Red-line 4: Converter flags must NEVER be inconsistent**
```cpp
// BAD: Inconsistent flags for special values
double_conversion::StringToDoubleConverter converter(
    double_conversion::StringToDoubleConverter::NO_FLAGS,
    0.0, 0.0, nullptr, nullptr); // Undefined behavior

// GOOD: Always provide valid strings for special values
double_conversion::StringToDoubleConverter converter(
    double_conversion::StringToDoubleConverter::ALLOW_SPECIAL_FLOATS,
    0.0, 0.0, "infinity", "not_a_number");
```

**Red-line 5: Thread safety must NEVER be assumed without synchronization**
```cpp
// BAD: Shared converter in multithreaded context
double_conversion::DoubleToStringConverter shared_converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);

void worker_thread() {
    char buffer[128];
    shared_converter.ToShortest(3.14, buffer, sizeof(buffer)); // Data race!
}

// GOOD: Thread-local converter
thread_local double_conversion::DoubleToStringConverter tls_converter(
    double_conversion::DoubleToStringConverter::NO_FLAGS,
    "inf", "nan", 'e', 6, 6);

void worker_thread() {
    char buffer[128];
    tls_converter.ToShortest(3.14, buffer, sizeof(buffer)); // Safe
}
```