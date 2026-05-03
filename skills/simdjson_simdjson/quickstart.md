# Quickstart

```cpp
#include <iostream>
#include "simdjson.h"
using namespace simdjson;

// Pattern 1: Basic parsing with ondemand::parser
void basic_parsing() {
    ondemand::parser parser;
    padded_string json = padded_string::load("data.json");
    ondemand::document doc = parser.iterate(json);
    std::cout << std::string_view(doc["name"]) << std::endl;
}

// Pattern 2: Accessing nested objects
void nested_access() {
    ondemand::parser parser;
    auto json = padded_string::load("config.json");
    auto doc = parser.iterate(json);
    uint64_t count = doc["metadata"]["total_count"];
    std::cout << "Count: " << count << std::endl;
}

// Pattern 3: Iterating over arrays
void array_iteration() {
    ondemand::parser parser;
    auto json = padded_string::load("items.json");
    auto doc = parser.iterate(json);
    ondemand::array items = doc["items"].get_array();
    for (auto item : items) {
        std::cout << std::string_view(item["id"]) << std::endl;
    }
}

// Pattern 4: Error handling without exceptions
void error_handling() {
    ondemand::parser parser;
    auto json = padded_string::load("data.json");
    auto doc = parser.iterate(json);
    auto error = doc["missing_key"].get_string();
    if (error.error()) {
        std::cerr << "Key not found: " << error.error() << std::endl;
    }
}

// Pattern 5: Minifying JSON
void minify_json() {
    padded_string json = padded_string::load("large.json");
    std::string minified;
    simdjson::minify(json.data(), json.size(), minified);
    std::cout << "Minified size: " << minified.size() << std::endl;
}

// Pattern 6: Validating UTF-8
void validate_utf8() {
    padded_string json = padded_string::load("data.json");
    bool valid = simdjson::validate_utf8(json.data(), json.size());
    std::cout << (valid ? "Valid UTF-8" : "Invalid UTF-8") << std::endl;
}

// Pattern 7: Parsing NDJSON (newline-delimited JSON)
void parse_ndjson() {
    ondemand::parser parser;
    auto json = padded_string::load("lines.json");
    ondemand::document_stream stream = parser.iterate_many(json);
    for (auto doc : stream) {
        std::cout << std::string_view(doc["line_id"]) << std::endl;
    }
}

// Pattern 8: Building JSON with builder
void build_json() {
    ondemand::builder::object obj;
    obj["name"] = "example";
    obj["value"] = 42;
    obj["active"] = true;
    std::string result = obj.to_json();
    std::cout << result << std::endl;
}
```