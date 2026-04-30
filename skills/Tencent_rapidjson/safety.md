# Safety

**Red-line Condition 1: NEVER access document after failed parse**

BAD:
```cpp
rapidjson::Document doc;
doc.Parse("null");
if (doc.HasParseError()) {
    std::cout << doc["key"].GetString(); // Undefined behavior
}
```

GOOD:
```cpp
rapidjson::Document doc;
doc.Parse("null");
if (doc.HasParseError()) {
    std::cerr << "Parse failed, cannot access document" << std::endl;
    return;
}
// Safe to access doc now
```

**Red-line Condition 2: NEVER use StringRef with non-static or temporary strings**

BAD:
```cpp
rapidjson::Document doc;
doc.SetObject();
std::string temp = "hello";
doc.AddMember("key", rapidjson::StringRef(temp.c_str()), doc.GetAllocator());
// temp goes out of scope, member points to freed memory
```

GOOD:
```cpp
rapidjson::Document doc;
doc.SetObject();
doc.AddMember("key", "hello", doc.GetAllocator()); // Uses StringRef internally but safe with string literal
// Or copy explicitly:
std::string temp = "hello";
rapidjson::Value val;
val.SetString(temp.c_str(), temp.length(), doc.GetAllocator());
doc.AddMember("key", val, doc.GetAllocator());
```

**Red-line Condition 3: NEVER modify document while iterating over its array/object**

BAD:
```cpp
rapidjson::Document doc;
doc.Parse("[1,2,3]");
for (auto& v : doc.GetArray()) {
    if (v.GetInt() == 2) {
        doc.Erase(doc.Begin() + 1); // Invalidates iterator
    }
}
```

GOOD:
```cpp
rapidjson::Document doc;
doc.Parse("[1,2,3]");
// Collect indices to remove first
std::vector<size_t> toRemove;
for (size_t i = 0; i < doc.GetArray().Size(); ++i) {
    if (doc[i].GetInt() == 2) {
        toRemove.push_back(i);
    }
}
// Remove in reverse order
for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
    doc.Erase(doc.Begin() + *it);
}
```

**Red-line Condition 4: NEVER assume floating-point values are exact**

BAD:
```cpp
rapidjson::Document doc;
doc.Parse("0.1");
double val = doc.GetDouble();
if (val == 0.1) { // May be false due to precision loss
    // ...
}
```

GOOD:
```cpp
rapidjson::Document doc;
doc.Parse("0.1");
double val = doc.GetDouble();
if (std::abs(val - 0.1) < 1e-10) { // Use epsilon comparison
    // ...
}
```

**Red-line Condition 5: NEVER share SchemaDocument across threads without synchronization**

BAD:
```cpp
rapidjson::SchemaDocument schema(schemaDoc);
// Multiple threads using same schema object
```

GOOD:
```cpp
// Option 1: Create per-thread copies
rapidjson::SchemaDocument schema(schemaDoc);
// Each thread creates its own SchemaValidator

// Option 2: Use mutex
std::mutex schemaMutex;
rapidjson::SchemaDocument schema(schemaDoc);
// Thread: { std::lock_guard<std::mutex> lock(schemaMutex); SchemaValidator validator(schema); }
```