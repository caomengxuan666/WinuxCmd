# Safety

```cpp
// RED LINE 1: NEVER use a nil UUID as a valid identifier
// BAD:
#include <uuid.h>

void process_user(const uuids::uuid& id) {
    if (id.is_nil()) {
        // This should be an error, not silently handled
        return;
    }
}

// GOOD:
#include <uuid.h>
#include <stdexcept>

void process_user(const uuids::uuid& id) {
    if (id.is_nil()) {
        throw std::invalid_argument("Nil UUID is not a valid user identifier");
    }
}
```

```cpp
// RED LINE 2: NEVER assume UUID generation is deterministic
// BAD:
#include <uuid.h>

uuids::uuid get_cached_uuid() {
    static uuids::uuid cached = uuids::uuid_system_generator{}();
    return cached; // Same UUID every time - defeats purpose
}

// GOOD:
#include <uuid.h>

uuids::uuid generate_new_uuid() {
    return uuids::uuid_system_generator{}(); // Fresh UUID each call
}
```

```cpp
// RED LINE 3: NEVER use UUIDs for security-critical randomness
// BAD:
#include <uuid.h>
#include <string>

std::string generate_api_key() {
    uuids::uuid id = uuids::uuid_system_generator{}();
    return uuids::to_string(id); // UUIDs are not cryptographically secure
}

// GOOD:
#include <random>
#include <string>

std::string generate_api_key() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    std::string key(32, '\0');
    for (auto& c : key) {
        c = static_cast<char>(dis(gen));
    }
    return key; // Use proper cryptographic RNG for security
}
```

```cpp
// RED LINE 4: NEVER modify UUID internal bytes directly
// BAD:
#include <uuid.h>
#include <cstring>

void corrupt_uuid(uuids::uuid& id) {
    std::memset(&id, 0, sizeof(id)); // Undefined behavior
}

// GOOD:
#include <uuid.h>

uuids::uuid reset_to_nil() {
    return uuids::uuid{}; // Properly create nil UUID
}
```

```cpp
// RED LINE 5: NEVER use UUIDs across different generator types interchangeably
// BAD:
#include <uuid.h>
#include <unordered_set>

void mix_generators() {
    uuids::uuid_system_generator sys_gen;
    uuids::uuid_random_generator rand_gen;
    
    std::unordered_set<uuids::uuid> ids;
    ids.insert(sys_gen());
    ids.insert(rand_gen()); // Different generators may produce different UUID versions
}

// GOOD:
#include <uuid.h>
#include <unordered_set>

void use_consistent_generator() {
    uuids::uuid_system_generator gen;
    
    std::unordered_set<uuids::uuid> ids;
    ids.insert(gen());
    ids.insert(gen()); // Consistent generator type
}
```