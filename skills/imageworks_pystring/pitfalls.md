# Pitfalls

**1. Forgetting to clear the output vector before split()**
```cpp
// BAD: Appends to existing vector
std::vector<std::string> parts;
parts.push_back("existing");
pystring::split("a,b", parts, ",");
// parts = {"existing", "a", "b"} - unexpected!

// GOOD: Clear first or use fresh vector
std::vector<std::string> parts;
pystring::split("a,b", parts, ",");
// parts = {"a", "b"}
```

**2. Assuming split() with no delimiter splits on whitespace**
```cpp
// BAD: Missing delimiter argument
std::vector<std::string> parts;
pystring::split("hello world", parts);
// Compilation error - wrong overload

// GOOD: Use split() with whitespace delimiter
std::vector<std::string> parts;
pystring::split("hello world", parts);
// Actually works - default splits on whitespace
```

**3. Not handling empty strings in split results**
```cpp
// BAD: Assuming no empty strings
std::vector<std::string> parts;
pystring::split("a,,b", parts, ",");
// parts = {"a", "", "b"} - empty string included!

// GOOD: Filter empty strings if needed
std::vector<std::string> parts;
pystring::split("a,,b", parts, ",");
parts.erase(std::remove_if(parts.begin(), parts.end(),
    [](const std::string& s) { return s.empty(); }), parts.end());
```

**4. Using find() with wrong return type**
```cpp
// BAD: Using int instead of size_t
int pos = pystring::find("hello", "world");
// pos = -1 (but could be large positive if found)

// GOOD: Use size_t and check against npos
size_t pos = pystring::find("hello", "world");
if (pos != std::string::npos) {
    // Found at position pos
}
```

**5. Modifying string while iterating over split results**
```cpp
// BAD: Modifying original string during iteration
std::string text = "a,b,c";
std::vector<std::string> parts;
pystring::split(text, parts, ",");
for (auto& part : parts) {
    text += part; // Undefined behavior!
}

// GOOD: Work with a copy
std::string text = "a,b,c";
std::vector<std::string> parts;
pystring::split(text, parts, ",");
std::string result;
for (const auto& part : parts) {
    result += part;
}
```

**6. Assuming replace() modifies in-place**
```cpp
// BAD: Expecting original to change
std::string text = "hello world";
pystring::replace(text, "world", "there");
// text is still "hello world"!

// GOOD: Capture return value
std::string text = "hello world";
std::string result = pystring::replace(text, "world", "there");
// result = "hello there"
```