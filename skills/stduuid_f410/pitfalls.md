# Pitfalls

```cpp
// BAD: Assuming UUID string parsing always succeeds
#include <uuid.h>
#include <iostream>

int main() {
    std::string invalid = "not-a-uuid";
    auto id = uuids::uuid::from_string(invalid);
    std::cout << "UUID: " << uuids::to_string(id.value()) << std::endl; // CRASH if invalid
    return 0;
}
```

```cpp
// GOOD: Always check if parsing succeeded
#include <uuid.h>
#include <iostream>

int main() {
    std::string invalid = "not-a-uuid";
    auto id = uuids::uuid::from_string(invalid);
    if (id.has_value()) {
        std::cout << "UUID: " << uuids::to_string(id.value()) << std::endl;
    } else {
        std::cout << "Invalid UUID string" << std::endl;
    }
    return 0;
}
```

```cpp
// BAD: Using default-constructed UUID as valid
#include <uuid.h>
#include <iostream>

int main() {
    uuids::uuid id; // Default constructed - all zeros
    if (id == uuids::uuid{}) {
        std::cout << "This is a nil UUID, not a valid one" << std::endl;
    }
    return 0;
}
```

```cpp
// GOOD: Always generate a proper UUID
#include <uuid.h>
#include <iostream>

int main() {
    uuids::uuid id = uuids::uuid_system_generator{}();
    if (!id.is_nil()) {
        std::cout << "Valid UUID: " << uuids::to_string(id) << std::endl;
    }
    return 0;
}
```

```cpp
// BAD: Assuming UUIDs are unique without checking
#include <uuid.h>
#include <set>

int main() {
    std::set<uuids::uuid> ids;
    for (int i = 0; i < 100; ++i) {
        ids.insert(uuids::uuid_system_generator{}());
    }
    // No check for duplicates - though unlikely, not guaranteed
    return 0;
}
```

```cpp
// GOOD: Handle potential duplicates explicitly
#include <uuid.h>
#include <set>
#include <iostream>

int main() {
    std::set<uuids::uuid> ids;
    for (int i = 0; i < 100; ++i) {
        auto [it, inserted] = ids.insert(uuids::uuid_system_generator{}());
        if (!inserted) {
            std::cout << "Duplicate UUID generated!" << std::endl;
        }
    }
    return 0;
}
```

```cpp
// BAD: Using UUID as string for comparison
#include <uuid.h>
#include <string>

int main() {
    uuids::uuid id1 = uuids::uuid_system_generator{}();
    uuids::uuid id2 = uuids::uuid_system_generator{}();
    
    std::string s1 = uuids::to_string(id1);
    std::string s2 = uuids::to_string(id2);
    
    if (s1 == s2) { // Inefficient - compare bytes directly
        // ...
    }
    return 0;
}
```

```cpp
// GOOD: Compare UUIDs directly
#include <uuid.h>

int main() {
    uuids::uuid id1 = uuids::uuid_system_generator{}();
    uuids::uuid id2 = uuids::uuid_system_generator{}();
    
    if (id1 == id2) { // Efficient byte comparison
        // ...
    }
    return 0;
}
```