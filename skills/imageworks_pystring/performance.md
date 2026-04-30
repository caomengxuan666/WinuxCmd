# Performance

**Allocation patterns**
```cpp
// Each pystring function creates new strings
std::string result = pystring::strip(
    pystring::lower(
        pystring::replace(input, " ", "_")
    )
);
// Three temporary strings created!

// Better: Chain operations to minimize temporaries
std::string temp1 = pystring::replace(input, " ", "_");
std::string temp2 = pystring::lower(temp1);
std::string result = pystring::strip(temp2);
```

**Pre-allocation strategies**
```cpp
// Slow: Repeated allocations
std::vector<std::string> parts;
for (int i = 0; i < 1000; ++i) {
    parts.push_back(pystring::strip("  item  "));
}

// Fast: Pre-allocate
std::vector<std::string> parts;
parts.reserve(1000);
for (int i = 0; i < 1000; ++i) {
    parts.push_back(pystring::strip("  item  "));
}
```

**Avoid unnecessary copies**
```cpp
// Slow: Copying large strings
std::string large = get_large_string();
std::string result = pystring::upper(large); // Copy

// Better: Move if source not needed
std::string large = get_large_string();
std::string result = pystring::upper(std::move(large));
```

**Benchmark considerations**
```cpp
// split() performance depends on delimiter count
// For simple delimiters, manual parsing may be faster
std::string data = "a,b,c,d,e";

// Pystring split
std::vector<std::string> parts;
pystring::split(data, parts, ",");

// Manual parsing (potentially faster for simple cases)
std::vector<std::string> manual;
size_t start = 0, end;
while ((end = data.find(',', start)) != std::string::npos) {
    manual.push_back(data.substr(start, end - start));
    start = end + 1;
}
manual.push_back(data.substr(start));
```