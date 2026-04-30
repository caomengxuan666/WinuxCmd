# Performance

**Memory Allocation Patterns**

```cpp
// Default allocator uses memory pool - fast for many small allocations
rapidjson::Document doc;

// Custom allocator for specific needs
rapidjson::MemoryPoolAllocator<> poolAlloc(4096); // 4KB initial buffer
rapidjson::Document doc(&poolAlloc);

// Reuse allocator for multiple documents
rapidjson::MemoryPoolAllocator<> sharedAlloc;
{
    rapidjson::Document doc1(&sharedAlloc);
    doc1.Parse(json1);
}
{
    rapidjson::Document doc2(&sharedAlloc); // Reuses memory from doc1
    doc2.Parse(json2);
}
```

**Parsing Performance Optimization**

```cpp
// Use kParseInsituFlag for in-place parsing (modifies input string)
char* mutableJson = strdup("{\"key\":\"value\"}");
rapidjson::Document doc;
doc.Parse<rapidjson::kParseInsituFlag>(mutableJson);
// Faster but modifies the input buffer

// Use kParseFullPrecisionFlag for exact floating-point
doc.Parse<rapidjson::kParseFullPrecisionFlag>(jsonString);
// Slower but more accurate

// Use kParseNumbersAsStringsFlag to avoid numeric conversion
doc.Parse<rapidjson::kParseNumbersAsStringsFlag>(jsonString);
// Faster if you don't need numeric values immediately
```

**String Handling Optimization**

```cpp
rapidjson::Document doc;
doc.SetObject();

// StringRef avoids copying for static strings
doc.AddMember("type", rapidjson::StringRef("static"), doc.GetAllocator());

// For dynamic strings, reserve space
std::string largeStr = getLargeString();
rapidjson::Value val;
val.SetString(largeStr.c_str(), largeStr.length(), doc.GetAllocator());
// Consider using StringRef if the source string outlives the document

// Avoid repeated string lookups
const char* key = "longKeyName";
if (doc.HasMember(key)) {
    const rapidjson::Value& val = doc[key]; // Cache reference
    // Use val multiple times
}
```

**Writer Performance**

```cpp
// Use StringBuffer for small outputs
rapidjson::StringBuffer buffer;
rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

// Use FileWriteStream for large outputs
rapidjson::FileWriteStream os(stdout);
rapidjson::Writer<rapidjson::FileWriteStream> writer2(os);

// Disable UTF-8 validation for known-valid input
writer.SetValidateEncoding(false); // Faster but unsafe

// Pre-allocate buffer
rapidjson::StringBuffer buffer;
buffer.Reserve(1024 * 1024); // Reserve 1MB
```

**Benchmarking Tips**

```cpp
// Measure parsing time
auto start = std::chrono::high_resolution_clock::now();
rapidjson::Document doc;
doc.Parse(jsonString);
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

// For fair comparison, warm up the allocator
rapidjson::Document warmup;
warmup.Parse(jsonString);
```