# Quickstart

```cpp
// Common usage patterns for RapidJSON

// Pattern 1: Parse JSON string into DOM
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

void parse_json() {
    const char* json = "{\"name\":\"John\",\"age\":30}";
    rapidjson::Document doc;
    doc.Parse(json);
    
    if (doc.HasParseError()) {
        std::cerr << "Parse error: " << doc.GetParseError() << std::endl;
        return;
    }
    
    std::cout << doc["name"].GetString() << std::endl;
}

// Pattern 2: Create JSON from scratch
void create_json() {
    rapidjson::Document doc;
    doc.SetObject();
    
    rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();
    
    doc.AddMember("name", "Alice", alloc);
    doc.AddMember("age", 25, alloc);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    std::cout << buffer.GetString() << std::endl;
}

// Pattern 3: Iterate over array
void iterate_array() {
    const char* json = "[1,2,3,4,5]";
    rapidjson::Document doc;
    doc.Parse(json);
    
    for (auto& v : doc.GetArray()) {
        std::cout << v.GetInt() << " ";
    }
}

// Pattern 4: Access nested objects
void access_nested() {
    const char* json = "{\"person\":{\"name\":\"Bob\",\"address\":{\"city\":\"NYC\"}}}";
    rapidjson::Document doc;
    doc.Parse(json);
    
    if (doc.HasMember("person") && doc["person"].HasMember("address")) {
        std::string city = doc["person"]["address"]["city"].GetString();
        std::cout << city << std::endl;
    }
}

// Pattern 5: SAX-style parsing
#include "rapidjson/reader.h"

struct MyHandler {
    bool Null() { return true; }
    bool Bool(bool) { return true; }
    bool Int(int i) { std::cout << "Int: " << i << std::endl; return true; }
    bool Uint(unsigned u) { std::cout << "Uint: " << u << std::endl; return true; }
    bool Double(double d) { std::cout << "Double: " << d << std::endl; return true; }
    bool String(const char* str, size_t length, bool) { 
        std::cout << "String: " << str << std::endl; 
        return true; 
    }
    bool StartObject() { return true; }
    bool Key(const char* str, size_t length, bool) { return true; }
    bool EndObject(size_t) { return true; }
    bool StartArray() { return true; }
    bool EndArray(size_t) { return true; }
};

void sax_parse() {
    const char* json = "{\"key\":\"value\"}";
    rapidjson::Reader reader;
    MyHandler handler;
    rapidjson::StringStream ss(json);
    reader.Parse(ss, handler);
}

// Pattern 6: Pretty printing
#include "rapidjson/prettywriter.h"

void pretty_print() {
    rapidjson::Document doc;
    doc.Parse("{\"a\":1,\"b\":2}");
    
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    std::cout << buffer.GetString() << std::endl;
}

// Pattern 7: JSON Pointer
#include "rapidjson/pointer.h"

void json_pointer() {
    rapidjson::Document doc;
    doc.Parse("{\"foo\":{\"bar\":42}}");
    
    rapidjson::Pointer ptr("/foo/bar");
    if (rapidjson::Value* v = ptr.Get(doc)) {
        std::cout << v->GetInt() << std::endl;
    }
}

// Pattern 8: Schema validation
#include "rapidjson/schema.h"

void schema_validation() {
    const char* schemaJson = "{\"type\":\"object\",\"properties\":{\"name\":{\"type\":\"string\"}}}";
    const char* dataJson = "{\"name\":\"test\"}";
    
    rapidjson::Document schemaDoc;
    schemaDoc.Parse(schemaJson);
    rapidjson::SchemaDocument schema(schemaDoc);
    
    rapidjson::Document data;
    data.Parse(dataJson);
    
    rapidjson::SchemaValidator validator(schema);
    if (data.Accept(validator)) {
        std::cout << "Valid!" << std::endl;
    } else {
        std::cerr << "Invalid!" << std::endl;
    }
}
```