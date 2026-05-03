# Lifecycle

### Construction and initialization
```cpp
// Default construction
Person person;  // All fields initialized to defaults

// Copy construction
Person original;
original.set_name("Alice");
Person copy(original);  // Deep copy

// Move construction (C++11+)
Person moved(std::move(copy));  // Efficient transfer
```

### Assignment and swap
```cpp
// Copy assignment
Person p1, p2;
p1.set_name("Alice");
p2 = p1;  // Deep copy

// Move assignment
Person p3;
p3 = std::move(p2);  // Efficient transfer

// Swap
p1.Swap(&p3);  // Fast pointer swap
```

### Destruction and cleanup
```cpp
// Automatic cleanup
{
    Person person;
    person.set_name("Bob");
    // person destroyed here, memory freed
}

// Manual cleanup (rarely needed)
Person* person = new Person();
person->set_name("Charlie");
delete person;  // Proper cleanup
```

### Resource management with arenas
```cpp
// Arena-based allocation
google::protobuf::Arena arena;

// Create messages on arena
Person* p1 = google::protobuf::Arena::CreateMessage<Person>(&arena);
Person* p2 = google::protobuf::Arena::CreateMessage<Person>(&arena);

// Arena handles all cleanup
// No need to call delete on p1 or p2
```

### Move semantics with protobuf
```cpp
// Efficient move operations
std::vector<Person> people;
Person temp;
temp.set_name("David");
people.push_back(std::move(temp));  // Move into vector

// After move, source is in valid but unspecified state
// temp.name() is empty or default
```

### Serialization lifecycle
```cpp
// Serialize to string
std::string serialized;
person.SerializeToString(&serialized);  // Allocates string

// Serialize to array (avoid allocation)
char buffer[1024];
person.SerializeToArray(buffer, sizeof(buffer));

// Deserialize
Person parsed;
parsed.ParseFromString(serialized);  // Allocates internal memory
```