# Safety

**1. NEVER pass null pointers to pystring functions**
```cpp
// BAD: Null pointer
const char* null_str = nullptr;
std::string result = pystring::strip(null_str); // Undefined behavior

// GOOD: Always use valid strings
std::string valid = "  hello  ";
std::string result = pystring::strip(valid);
```

**2. NEVER assume thread safety without external synchronization**
```cpp
// BAD: Concurrent access to shared string
std::string shared = "hello";
std::thread t1([&]() { pystring::upper(shared); });
std::thread t2([&]() { pystring::lower(shared); });
t1.join(); t2.join(); // Data race!

// GOOD: Use mutex or local copies
std::string shared = "hello";
std::mutex mtx;
std::thread t1([&]() { 
    std::lock_guard<std::mutex> lock(mtx);
    pystring::upper(shared); 
});
```

**3. NEVER ignore return values from functions that return new strings**
```cpp
// BAD: Ignoring return value
std::string text = "  hello  ";
pystring::strip(text); // Return value discarded!

// GOOD: Always capture the result
std::string text = "  hello  ";
std::string stripped = pystring::strip(text);
```

**4. NEVER use split() with an empty delimiter string**
```cpp
// BAD: Empty delimiter
std::vector<std::string> parts;
pystring::split("hello", parts, ""); // Undefined behavior

// GOOD: Use valid delimiter
std::vector<std::string> parts;
pystring::split("hello", parts, " "); // Split on space
```

**5. NEVER assume the output vector is cleared by split()**
```cpp
// BAD: Assuming split() clears the vector
std::vector<std::string> parts = {"existing"};
pystring::split("a,b", parts, ",");
// parts now contains both old and new elements!

// GOOD: Clear explicitly if needed
std::vector<std::string> parts = {"existing"};
parts.clear();
pystring::split("a,b", parts, ",");
```