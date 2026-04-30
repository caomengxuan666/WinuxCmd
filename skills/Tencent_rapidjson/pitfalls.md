# Pitfalls

**Pitfall 1: Not checking parse errors**

BAD:
```cpp
rapidjson::Document doc;
doc.Parse("invalid json");
std::cout << doc["key"].GetString(); // Crashes if parse failed
```

GOOD:
```cpp
rapidjson::Document doc;
doc.Parse("invalid json");
if (doc.HasParseError()) {
    std::cerr << "Parse error: " << doc.GetParseError() << std::endl;
    return;
}
std::cout << doc["key"].GetString();
```

**Pitfall 2: Using AddMember with temporary values incorrectly**

BAD:
```cpp
rapidjson::Document doc;
doc.SetObject();
rapidjson::Value val;
val.SetString("temp"); // No allocator provided
doc.AddMember("key", val, doc.GetAllocator()); // val may be invalid
```

GOOD:
```cpp
rapidjson::Document doc;
doc.SetObject();
rapidjson::Value val;
val.SetString("temp", doc.GetAllocator());
doc.AddMember("key", val, doc.GetAllocator());
```

**Pitfall 3: Accessing members without checking existence**

BAD:
```cpp
rapidjson::Document doc;
doc.Parse("{\"name\":\"test\"}");
int age = doc["age"].GetInt(); // Crashes if "age" doesn't exist
```

GOOD:
```cpp
rapidjson::Document doc;
doc.Parse("{\"name\":\"test\"}");
if (doc.HasMember("age") && doc["age"].IsInt()) {
    int age = doc["age"].GetInt();
}
```

**Pitfall 4: Assuming floating-point precision**

BAD:
```cpp
rapidjson::Document doc;
doc.Parse("0.9868011474609375");
double val = doc.GetDouble();
// val may be 0.9868011474609376 due to parsing optimization
```

GOOD:
```cpp
rapidjson::Document doc;
doc.Parse<rapidjson::kParseNumbersAsStringsFlag>("0.9868011474609375");
std::string val = doc.GetString(); // Preserve exact representation
```

**Pitfall 5: Using StringRef with temporary strings**

BAD:
```cpp
rapidjson::Document doc;
std::string getTempString() { return "temporary"; }
doc.SetString(rapidjson::StringRef(getTempString().c_str())); // dangling pointer
```

GOOD:
```cpp
rapidjson::Document doc;
std::string temp = getTempString();
doc.SetString(temp.c_str(), doc.GetAllocator()); // Copy the string
```

**Pitfall 6: Modifying document while iterating**

BAD:
```cpp
rapidjson::Document doc;
doc.Parse("[1,2,3]");
for (auto& v : doc.GetArray()) {
    doc.PushBack(4, doc.GetAllocator()); // Invalidates iterators
}
```

GOOD:
```cpp
rapidjson::Document doc;
doc.Parse("[1,2,3]");
rapidjson::Value newVal(4);
doc.PushBack(newVal, doc.GetAllocator()); // Modify after iteration
```

**Pitfall 7: Using SchemaDocument in multi-threaded context without synchronization**

BAD:
```cpp
// Shared SchemaDocument accessed from multiple threads
rapidjson::SchemaDocument schema(schemaDoc);
// Thread 1: SchemaValidator validator(schema);
// Thread 2: SchemaValidator validator(schema); // May cause heap corruption
```

GOOD:
```cpp
// Create separate SchemaDocument per thread or use mutex
std::mutex schemaMutex;
rapidjson::SchemaDocument schema(schemaDoc);
// Thread 1: { std::lock_guard<std::mutex> lock(schemaMutex); SchemaValidator validator(schema); }
// Thread 2: { std::lock_guard<std::mutex> lock(schemaMutex); SchemaValidator validator(schema); }
```