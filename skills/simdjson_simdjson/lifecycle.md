# Lifecycle

**Construction and Destruction**

The simdjson library has three main objects with distinct lifecycles:

```cpp
// Parser: lightweight, can be reused
ondemand::parser parser; // Default construction

// padded_string: owns the buffer
padded_string json = padded_string::load("data.json"); // From file
padded_string from_str("raw string"); // From string literal

// Document: references the parser and padded_string
auto doc = parser.iterate(json); // doc is valid as long as parser and json exist
```

**Move Semantics**

All major types support move semantics for efficient transfer:

```cpp
// Moving parser
ondemand::parser parser1;
ondemand::parser parser2 = std::move(parser1); // parser1 is now empty

// Moving padded_string
padded_string json1 = padded_string::load("data.json");
padded_string json2 = std::move(json1); // json1 is now empty

// Moving document
auto doc1 = parser.iterate(json2);
auto doc2 = std::move(doc1); // doc1 is now invalid
```

**Resource Management**

```cpp
void proper_lifecycle() {
    // 1. Create parser (cheap)
    ondemand::parser parser;
    
    // 2. Load data (allocates buffer)
    padded_string json = padded_string::load("large.json");
    
    // 3. Parse (creates document referencing parser and json)
    auto doc = parser.iterate(json);
    
    // 4. Use document
    auto value = doc["key"].get_string();
    
    // 5. Destroy in reverse order
    // doc goes out of scope first (optional, but good practice)
    // json goes out of scope (frees buffer)
    // parser goes out of scope (frees internal state)
}
```

**Reusing Parser**

```cpp
ondemand::parser parser;

void process_multiple_files() {
    // First file
    {
        padded_string json = padded_string::load("file1.json");
        auto doc = parser.iterate(json);
        process(doc);
    } // doc destroyed, parser can be reused
    
    // Second file
    {
        padded_string json = padded_string::load("file2.json");
        auto doc = parser.iterate(json); // Safe reuse
        process(doc);
    }
}
```

**Copy Semantics**

simdjson objects are generally move-only:

```cpp
// This will NOT compile:
ondemand::parser parser1;
ondemand::parser parser2 = parser1; // Error: no copy constructor

// This will NOT compile:
padded_string json1 = padded_string::load("data.json");
padded_string json2 = json1; // Error: no copy constructor

// You must explicitly copy data:
padded_string json1 = padded_string::load("data.json");
padded_string json2(json1.data(), json1.size()); // Manual copy
```