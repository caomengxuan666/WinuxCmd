# Best Practices

**1. Use const references for input strings**
```cpp
// Good practice: Pass by const reference
void process_string(const std::string& input) {
    std::vector<std::string> parts;
    pystring::split(input, parts, ",");
    // Process parts...
}
```

**2. Pre-allocate vectors for performance**
```cpp
// Good practice: Reserve space when you know the size
std::string csv = "a,b,c,d,e";
std::vector<std::string> parts;
parts.reserve(std::count(csv.begin(), csv.end(), ',') + 1);
pystring::split(csv, parts, ",");
```

**3. Chain operations for readability**
```cpp
// Good practice: Chain operations
std::string result = pystring::strip(
    pystring::lower(
        pystring::replace(input, "OLD", "NEW")
    )
);
```

**4. Use startswith/endswith for file extension checks**
```cpp
// Good practice: File extension validation
bool is_valid_file(const std::string& filename) {
    return pystring::endswith(filename, ".txt") ||
           pystring::endswith(filename, ".csv") ||
           pystring::endswith(filename, ".json");
}
```

**5. Combine split with other operations**
```cpp
// Good practice: Parse and transform
std::string parse_and_transform(const std::string& input) {
    std::vector<std::string> parts;
    pystring::split(input, parts, "|");
    
    std::vector<std::string> cleaned;
    cleaned.reserve(parts.size());
    for (const auto& part : parts) {
        cleaned.push_back(pystring::strip(part));
    }
    
    return pystring::join(", ", cleaned);
}
```