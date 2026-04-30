# Lifecycle

**Construction and Destruction**

```cpp
// Default construction creates a null value
rapidjson::Document doc; // doc.IsNull() == true

// Construction with type
rapidjson::Value v1; // null
rapidjson::Value v2(42); // int
rapidjson::Value v3(3.14); // double
rapidjson::Value v4(true); // bool
rapidjson::Value v5("hello"); // string (StringRef)
rapidjson::Value v6(rapidjson::kObjectType); // empty object
rapidjson::Value v7(rapidjson::kArrayType); // empty array

// Destruction is automatic when Value goes out of scope
// Memory is freed by the allocator
```

**Move Semantics**

```cpp
rapidjson::Document doc1;
doc1.SetObject();
doc1.AddMember("key", "value", doc1.GetAllocator());

// Move constructor - transfers ownership
rapidjson::Document doc2(std::move(doc1));
// doc1 is now in a valid but unspecified state

// Move assignment
rapidjson::Document doc3;
doc3 = std::move(doc2);

// Move from Value
rapidjson::Value source(42);
rapidjson::Value dest(std::move(source)); // dest now holds 42, source is null
```

**Resource Management**

```cpp
// Allocator management
rapidjson::Document doc;
rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();

// Memory is allocated from the allocator
rapidjson::Value* v = new (alloc.Malloc(sizeof(rapidjson::Value))) rapidjson::Value(42);
// No need to manually free - allocator handles it

// Custom allocator
rapidjson::MemoryPoolAllocator<> customAlloc;
rapidjson::Document doc(&customAlloc);

// Clear document to release memory
doc.RemoveAllMembers(); // Removes members but doesn't release memory
doc.SetNull(); // Reset to null, memory may be reused
```

**Copy vs Move**

```cpp
rapidjson::Document source;
source.SetObject();
source.AddMember("data", 42, source.GetAllocator());

// Copy (requires allocator)
rapidjson::Document dest;
dest.CopyFrom(source, dest.GetAllocator());

// Move (cheaper)
rapidjson::Document dest2(std::move(source));

// Deep copy of Value
rapidjson::Value val(42);
rapidjson::Value copiedVal;
copiedVal.CopyFrom(val, dest.GetAllocator());
```