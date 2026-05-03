# Best Practices

**Best Practice 1: Always use padded_string for input**

```cpp
// Always wrap raw strings or file contents in padded_string
padded_string json = padded_string::load("config.json");
// Or from a string:
std::string raw = R"({"key": "value"})";
padded_string padded(raw);
```

**Best Practice 2: Scope document usage to prevent dangling references**

```cpp
void process_config() {
    padded_string json = padded_string::load("config.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    
    // Extract all needed values as copies
    std::string name = std::string(doc["name"].get_string().value());
    uint64_t version = doc["version"].get_uint64().value();
    
    // json and doc go out of scope here, but we have copies
    use_config(name, version);
}
```

**Best Practice 3: Use on-demand parsing for selective access**

```cpp
// Only parse what you need
void get_specific_field(const std::string& filename) {
    padded_string json = padded_string::load(filename);
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    
    // Skip to the field you need without parsing the whole document
    auto result = doc["deeply"]["nested"]["field"].get_string();
    if (!result.error()) {
        std::cout << result.value() << std::endl;
    }
}
```

**Best Practice 4: Handle errors consistently**

```cpp
simdjson_result<ondemand::document> safe_parse(const std::string& filename) {
    padded_string json;
    auto load_error = padded_string::load(filename).get(json);
    if (load_error) {
        return load_error;
    }
    
    ondemand::parser parser;
    ondemand::document doc;
    auto parse_error = parser.iterate(json).get(doc);
    if (parse_error) {
        return parse_error;
    }
    
    return doc;
}
```

**Best Practice 5: Use iterate_many for NDJSON streams**

```cpp
void process_ndjson_file(const std::string& filename) {
    padded_string json = padded_string::load(filename);
    ondemand::parser parser;
    
    int line_num = 0;
    for (auto result : parser.iterate_many(json)) {
        if (result.error()) {
            std::cerr << "Error on line " << line_num << ": " << result.error() << std::endl;
            continue;
        }
        auto doc = result.value();
        process_document(doc, line_num++);
    }
}
```

**Best Practice 6: Cache parser objects for repeated use**

```cpp
class JsonProcessor {
    ondemand::parser parser;
    
public:
    void process_file(const std::string& filename) {
        padded_string json = padded_string::load(filename);
        auto doc = parser.iterate(json);
        // Process doc
    } // doc destroyed, parser can be reused
};
```