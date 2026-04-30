# Quickstart

```cpp
// Example 1: Basic UUID generation
#include <uuid.h>
#include <iostream>

int main() {
    uuids::uuid id = uuids::uuid_system_generator{}();
    std::cout << "Generated UUID: " << uuids::to_string(id) << std::endl;
    return 0;
}
```

```cpp
// Example 2: UUID from string
#include <uuid.h>
#include <iostream>

int main() {
    std::string str = "550e8400-e29b-41d4-a716-446655440000";
    auto id = uuids::uuid::from_string(str);
    if (id.has_value()) {
        std::cout << "Parsed UUID: " << uuids::to_string(id.value()) << std::endl;
    }
    return 0;
}
```

```cpp
// Example 3: UUID comparison
#include <uuid.h>
#include <iostream>

int main() {
    uuids::uuid id1 = uuids::uuid_system_generator{}();
    uuids::uuid id2 = uuids::uuid_system_generator{}();
    
    if (id1 == id2) {
        std::cout << "Same UUID" << std::endl;
    } else {
        std::cout << "Different UUIDs" << std::endl;
    }
    return 0;
}
```

```cpp
// Example 4: UUID as map key
#include <uuid.h>
#include <map>
#include <string>

int main() {
    std::map<uuids::uuid, std::string> users;
    uuids::uuid id = uuids::uuid_system_generator{}();
    users[id] = "John Doe";
    
    auto it = users.find(id);
    if (it != users.end()) {
        std::cout << "Found user: " << it->second << std::endl;
    }
    return 0;
}
```

```cpp
// Example 5: UUID serialization
#include <uuid.h>
#include <sstream>

int main() {
    uuids::uuid id = uuids::uuid_system_generator{}();
    std::stringstream ss;
    ss << id;
    std::string serialized = ss.str();
    std::cout << "Serialized: " << serialized << std::endl;
    return 0;
}
```

```cpp
// Example 6: UUID from bytes
#include <uuid.h>
#include <array>
#include <iostream>

int main() {
    std::array<uint8_t, 16> bytes = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                     0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    uuids::uuid id(bytes);
    std::cout << "UUID from bytes: " << uuids::to_string(id) << std::endl;
    return 0;
}
```