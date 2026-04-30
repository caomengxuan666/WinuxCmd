# Quickstart

```cpp
#include <iostream>
#include "pystring.h"

int main() {
    // 1. String splitting
    std::string text = "apple,banana,cherry";
    std::vector<std::string> parts;
    pystring::split(text, parts, ",");
    // parts = {"apple", "banana", "cherry"}

    // 2. String joining
    std::vector<std::string> words = {"hello", "world"};
    std::string joined = pystring::join(", ", words);
    // joined = "hello, world"

    // 3. String stripping
    std::string padded = "   hello world   ";
    std::string stripped = pystring::strip(padded);
    // stripped = "hello world"

    // 4. String replacement
    std::string original = "Hello World";
    std::string replaced = pystring::replace(original, "World", "Universe");
    // replaced = "Hello Universe"

    // 5. String finding
    std::string search = "Hello World World";
    size_t pos = pystring::find(search, "World");
    // pos = 6 (first occurrence)

    // 6. String counting
    std::string count_text = "ababab";
    size_t count = pystring::count(count_text, "ab");
    // count = 3

    // 7. String starts/ends with
    std::string check = "hello.txt";
    bool starts = pystring::startswith(check, "hello");
    bool ends = pystring::endswith(check, ".txt");
    // starts = true, ends = true

    // 8. String case conversion
    std::string mixed = "Hello World";
    std::string lower = pystring::lower(mixed);
    std::string upper = pystring::upper(mixed);
    // lower = "hello world", upper = "HELLO WORLD"

    return 0;
}
```