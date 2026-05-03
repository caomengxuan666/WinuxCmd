# Best Practices

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// 1. Use value() with defaults for optional fields
json config = json::parse(config_string);
std::string host = config.value("host", "localhost");
int port = config.value("port", 8080);

// 2. Prefer at() over operator[] for const access
const json data = get_data();
try {
    auto val = data.at("required_field");
} catch (const json::out_of_range& e) {
    // Handle missing required field
}

// 3. Use structured bindings for iteration
json users = json::parse(users_json);
for (const auto& [id, user] : users.items()) {
    std::cout << "User " << id << ": " << user["name"] << '\n';
}

// 4. Create custom type conversions
namespace ns {
    struct Person {
        std::string name;
        int age;
    };
    
    void to_json(json& j, const Person& p) {
        j = {{"name", p.name}, {"age", p.age}};
    }
    
    void from_json(const json& j, Person& p) {
        j.at("name").get_to(p.name);
        j.at("age").get_to(p.age);
    }
}

// 5. Use SAX interface for streaming large files
struct MyHandler : json::json_sax_t {
    bool key(std::string& val) override {
        current_key = val;
        return true;
    }
    bool number_integer(std::int64_t val) override {
        if (current_key == "id") {
            ids.push_back(val);
        }
        return true;
    }
    std::string current_key;
    std::vector<std::int64_t> ids;
};

// 6. Prefer emplace_back for arrays
json arr = json::array();
arr.emplace_back("value1");
arr.emplace_back(42);

// 7. Use merge_patch for partial updates
json base = {{"a", 1}, {"b", 2}};
json patch = {{"b", 3}, {"c", 4}};
base.merge_patch(patch); // base becomes {{"a", 1}, {"b", 3}, {"c", 4}}
```