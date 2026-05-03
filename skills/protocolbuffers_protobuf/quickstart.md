# Quickstart

```cpp
// Basic serialization and deserialization
#include <iostream>
#include <fstream>
#include "person.pb.h"

int main() {
    // Create a Person message
    Person person;
    person.set_name("Alice");
    person.set_id(123);
    person.set_email("alice@example.com");
    
    // Serialize to string
    std::string serialized;
    person.SerializeToString(&serialized);
    
    // Deserialize from string
    Person parsed;
    parsed.ParseFromString(serialized);
    std::cout << "Name: " << parsed.name() << std::endl;
    return 0;
}
```

```cpp
// Working with repeated fields
#include "addressbook.pb.h"

int main() {
    AddressBook book;
    
    // Add multiple people
    Person* person1 = book.add_people();
    person1->set_name("Bob");
    person1->set_id(1);
    
    Person* person2 = book.add_people();
    person2->set_name("Carol");
    person2->set_id(2);
    
    // Iterate over repeated field
    for (int i = 0; i < book.people_size(); ++i) {
        const Person& p = book.people(i);
        std::cout << p.name() << std::endl;
    }
    return 0;
}
```

```cpp
// Reading/writing to file
#include <fstream>
#include "mydata.pb.h"

void SaveToFile(const MyData& data, const std::string& filename) {
    std::fstream output(filename, std::ios::out | std::ios::binary);
    data.SerializeToOstream(&output);
}

void LoadFromFile(MyData* data, const std::string& filename) {
    std::fstream input(filename, std::ios::in | std::ios::binary);
    data->ParseFromIstream(&input);
}
```

```cpp
// Using enums from proto definitions
#include "status.pb.h"

int main() {
    StatusMessage msg;
    msg.set_status(StatusCode::SUCCESS);
    msg.set_description("Operation completed");
    
    // Check enum value
    if (msg.status() == StatusCode::SUCCESS) {
        std::cout << "Success!" << std::endl;
    }
    return 0;
}
```

```cpp
// Working with nested messages
#include "config.pb.h"

int main() {
    Config config;
    config.set_app_name("MyApp");
    
    // Access nested message
    Config::Database* db = config.mutable_database();
    db->set_host("localhost");
    db->set_port(5432);
    db->set_username("admin");
    
    // Read nested message
    std::cout << "DB Host: " << config.database().host() << std::endl;
    return 0;
}
```

```cpp
// Using oneof fields
#include "request.pb.h"

int main() {
    Request req;
    
    // Set one of the oneof fields
    req.mutable_query("SELECT * FROM users");
    // OR
    req.set_command("SHUTDOWN");
    
    // Check which field is set
    if (req.has_query()) {
        std::cout << "Query: " << req.query() << std::endl;
    } else if (req.has_command()) {
        std::cout << "Command: " << req.command() << std::endl;
    }
    return 0;
}
```

```cpp
// Using map fields
#include "inventory.pb.h"

int main() {
    Inventory inv;
    
    // Add items to map
    (*inv.mutable_items())["apple"] = 10;
    (*inv.mutable_items())["banana"] = 5;
    
    // Iterate over map
    for (const auto& pair : inv.items()) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
```

```cpp
// JSON serialization (requires protobuf/util)
#include <google/protobuf/util/json_util.h>
#include "data.pb.h"

int main() {
    Data data;
    data.set_value(42);
    
    std::string json;
    google::protobuf::util::MessageToJsonString(data, &json);
    
    Data parsed;
    google::protobuf::util::JsonStringToMessage(json, &parsed);
    return 0;
}
```