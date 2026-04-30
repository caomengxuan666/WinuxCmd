# Lifecycle

**Construction and initialization**
```cpp
#include "pystring.h"

// Strings are created normally
std::string s1 = "hello";
std::string s2(100, 'x'); // Pre-allocate for performance

// Pystring functions return new strings
std::string result = pystring::upper(s1);
```

**Move semantics for efficiency**
```cpp
// Good: Use move semantics to avoid copies
std::string get_processed() {
    std::string temp = "  hello world  ";
    std::string stripped = pystring::strip(temp);
    return stripped; // NRVO or move
}

// Explicit move when done with source
std::string source = "HELLO";
std::string lower = pystring::lower(std::move(source));
// source is now in valid but unspecified state
```

**Resource management with vectors**
```cpp
// Vectors of strings from split
std::vector<std::string> parts;
pystring::split("a,b,c", parts, ",");

// Clear and reuse
parts.clear();
pystring::split("x,y,z", parts, ",");

// Swap to free memory
std::vector<std::string>().swap(parts);
```

**Destruction and cleanup**
```cpp
// Strings clean up automatically
{
    std::string temp = pystring::repeat("ab", 1000);
    // temp holds 2000 characters
} // temp destroyed, memory freed

// Vector cleanup
std::vector<std::string> results;
pystring::split(large_text, results, "\n");
// Use results...
results.clear(); // Clear contents
results.shrink_to_fit(); // Release memory
```