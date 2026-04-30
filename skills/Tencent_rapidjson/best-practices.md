# Best Practices

**Always check parse errors and type validity**

```cpp
rapidjson::Document doc;
doc.Parse(jsonString);
if (doc.HasParseError()) {
    // Handle error
    return;
}

// Always check type before access
if (doc.IsObject() && doc.HasMember("value") && doc["value"].IsInt()) {
    int val = doc["value"].GetInt();
}
```

**Use allocator-aware string creation for dynamic strings**

```cpp
rapidjson::Document doc;
doc.SetObject();
rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();

// For dynamic strings, always provide allocator
std::string dynamicStr = getUserInput();
rapidjson::Value val;
val.SetString(dynamicStr.c_str(), dynamicStr.length(), alloc);
doc.AddMember("input", val, alloc);
```

**Prefer StringRef for static strings to avoid allocation**

```cpp
rapidjson::Document doc;
doc.SetObject();

// Static string literals are safe with StringRef
doc.AddMember("type", rapidjson::StringRef("static_value"), doc.GetAllocator());
// This avoids copying the string
```

**Use kParseNumbersAsStringsFlag for precise numeric handling**

```cpp
rapidjson::Document doc;
doc.Parse<rapidjson::kParseNumbersAsStringsFlag>(jsonString);
// All numbers are stored as strings, preserving exact representation
// Convert manually when needed:
if (doc["value"].IsString()) {
    double val = std::stod(doc["value"].GetString());
}
```

**Implement proper error handling in SAX handlers**

```cpp
struct SafeHandler {
    bool errorOccurred = false;
    
    bool Null() { return true; }
    bool Bool(bool) { return true; }
    bool Int(int) { return true; }
    bool Uint(unsigned) { return true; }
    bool Double(double) { return true; }
    bool String(const char*, size_t, bool) { return true; }
    bool StartObject() { return true; }
    bool Key(const char*, size_t, bool) { return true; }
    bool EndObject(size_t) { return true; }
    bool StartArray() { return true; }
    bool EndArray(size_t) { return true; }
    
    bool Default() { 
        errorOccurred = true;
        return false; // Stop parsing on unknown event
    }
};
```

**Use PrettyWriter for human-readable output**

```cpp
rapidjson::Document doc;
doc.Parse(jsonString);

rapidjson::StringBuffer buffer;
rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
writer.SetIndent(' ', 2); // 2-space indentation
doc.Accept(writer);

std::cout << buffer.GetString() << std::endl;
```