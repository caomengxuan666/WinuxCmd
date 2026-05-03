# Best Practices

### Use factory functions for message creation
```cpp
// Good: Encapsulate creation logic
class MessageFactory {
public:
    static Person CreatePerson(const std::string& name, int id) {
        Person person;
        person.set_name(name);
        person.set_id(id);
        return person;
    }
    
    static std::unique_ptr<Person> CreatePersonPtr(const std::string& name, int id) {
        auto person = std::make_unique<Person>();
        person->set_name(name);
        person->set_id(id);
        return person;
    }
};
```

### Prefer arena allocation for performance
```cpp
// Good: Use arena for many small allocations
#include <google/protobuf/arena.h>

void ProcessBatch() {
    google::protobuf::Arena arena;
    
    // Create messages on arena
    Person* p1 = google::protobuf::Arena::CreateMessage<Person>(&arena);
    Person* p2 = google::protobuf::Arena::CreateMessage<Person>(&arena);
    
    p1->set_name("Alice");
    p2->set_name("Bob");
    
    // No need to delete - arena handles cleanup
}
```

### Validate messages before serialization
```cpp
// Good: Use reflection for validation
bool ValidatePerson(const Person& person) {
    const auto* reflection = person.GetReflection();
    const auto* descriptor = person.GetDescriptor();
    
    // Check required fields (proto2)
    for (int i = 0; i < descriptor->field_count(); ++i) {
        const auto* field = descriptor->field(i);
        if (field->is_required() && !reflection->HasField(person, field)) {
            return false;
        }
    }
    return true;
}
```

### Use lazy parsing for selective field access
```cpp
// Good: Parse only needed fields
void ProcessLargeMessage(const std::string& data) {
    LargeMessage msg;
    msg.ParsePartialFromString(data);  // Parse only what's needed
    
    // Access only specific fields
    if (msg.has_important_field()) {
        // Process important field
    }
}
```

### Implement proper error handling chains
```cpp
// Good: Consistent error handling
class ProtoHandler {
public:
    Result<Person> ParsePerson(const std::string& data) {
        if (data.empty()) {
            return Error("Empty data");
        }
        
        Person person;
        if (!person.ParseFromString(data)) {
            return Error("Parse failed");
        }
        
        if (!ValidatePerson(person)) {
            return Error("Validation failed");
        }
        
        return person;
    }
};
```

### Use custom options for metadata
```cpp
// Good: Extend proto with custom options
extend google.protobuf.FieldOptions {
  optional string validation_regex = 50000;
}

message User {
  string email = 1 [(validation_regex) = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"];
}
```