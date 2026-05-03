# Pitfalls

### Pitfall 1: Forgetting to check parse return value
```cpp
// BAD: Ignoring parse failure
std::string data = GetData();
Person person;
person.ParseFromString(data);  // Returns bool, ignored!
// person may be in undefined state

// GOOD: Always check parse result
std::string data = GetData();
Person person;
if (!person.ParseFromString(data)) {
    throw std::runtime_error("Failed to parse Person");
}
```

### Pitfall 2: Modifying while iterating
```cpp
// BAD: Modifying repeated field during iteration
for (int i = 0; i < book.people_size(); ++i) {
    if (book.people(i).name() == "Bob") {
        book.mutable_people()->DeleteSubrange(i, 1);  // Invalidates iterator
    }
}

// GOOD: Collect indices first, then remove
std::vector<int> to_remove;
for (int i = 0; i < book.people_size(); ++i) {
    if (book.people(i).name() == "Bob") {
        to_remove.push_back(i);
    }
}
for (auto it = to_remove.rbegin(); it != to_remove.rend(); ++it) {
    book.mutable_people()->DeleteSubrange(*it, 1);
}
```

### Pitfall 3: Using mutable accessor on const object
```cpp
// BAD: Compiler error or undefined behavior
const Person person = GetPerson();
person.mutable_name()->append(" Jr.");  // Error: can't modify const

// GOOD: Work with mutable copy
Person person = GetPerson();
person.mutable_name()->append(" Jr.");
```

### Pitfall 4: Ignoring field presence in proto3
```cpp
// BAD: Assuming default values mean "not set"
if (person.age() == 0) {  // Could be 0 or not set
    // Handle missing age
}

// GOOD: Use explicit presence checks (proto3 optional)
message Person {
  optional int32 age = 1;
}
// Then check:
if (person.has_age()) {
    // Age is explicitly set
}
```

### Pitfall 5: Memory leaks with mutable accessors
```cpp
// BAD: Creating dangling pointers
Person* p = message.mutable_person();
delete p;  // WRONG! protobuf owns the memory

// GOOD: Just use the pointer, don't delete
Person* p = message.mutable_person();
p->set_name("Alice");
```

### Pitfall 6: Serializing large messages to string
```cpp
// BAD: Copying large data multiple times
std::string huge_data;
message.SerializeToString(&huge_data);  // Copies entire message

// GOOD: Use stream or array output
std::string buffer;
buffer.resize(message.ByteSizeLong());
message.SerializeToArray(buffer.data(), buffer.size());
```

### Pitfall 7: Ignoring field number conflicts
```cpp
// BAD: Reusing field numbers
message User {
  string name = 1;
  // Later: removed name, added email with same number
  string email = 1;  // BREAKS backward compatibility!
}

// GOOD: Reserve old field numbers
message User {
  reserved 1;  // Previously used by 'name'
  string email = 2;
}
```

### Pitfall 8: Not handling unknown fields
```cpp
// BAD: Discarding unknown fields during merge
Person merged;
merged.MergeFrom(partial);  // Unknown fields in partial are lost

// GOOD: Preserve unknown fields
Person merged;
merged.MergeFrom(partial);
// Or use reflection to check
const Reflection& reflection = *merged.GetReflection();
if (reflection.GetUnknownFields(merged).field_count() > 0) {
    // Handle unknown fields
}
```