# Best Practices

**Use absl::flat_hash_map for high-performance lookups**

```cpp
#include "absl/container/flat_hash_map.h"

// Pre-allocate capacity for better performance
absl::flat_hash_map<std::string, int> cache;
cache.reserve(1000); // Reserve space for 1000 elements

// Use try_emplace for efficient insertion
auto [it, inserted] = cache.try_emplace("key", 42);
```

**Use absl::StrCat for efficient string concatenation**

```cpp
#include "absl/strings/str_cat.h"

std::string result = absl::StrCat("Hello", " ", "World", "!", 2024);
// Much more efficient than multiple operator+ calls
```

**Use absl::StatusOr for functions that can fail**

```cpp
#include "absl/status/statusor.h"

absl::StatusOr<std::vector<int>> parse_numbers(const std::string& input) {
    std::vector<int> result;
    for (const auto& part : absl::StrSplit(input, ',')) {
        int num;
        if (!absl::SimpleAtoi(part, &num)) {
            return absl::InvalidArgumentError(
                absl::StrCat("Invalid number: ", part));
        }
        result.push_back(num);
    }
    return result;
}
```

**Use absl::Time for all time-related operations**

```cpp
#include "absl/time/time.h"

// Use absl::Time for absolute points in time
absl::Time deadline = absl::Now() + absl::Seconds(30);

// Use absl::Duration for time intervals
absl::Duration timeout = absl::Milliseconds(500);

// Format times consistently
std::string formatted = absl::FormatTime(deadline, absl::UTCTimeZone());
```

**Use absl::Cleanup for RAII-style cleanup**

```cpp
#include "absl/cleanup/cleanup.h"

void process_file(const std::string& filename) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        LOG(ERROR) << "Failed to open file";
        return;
    }
    auto cleanup = absl::MakeCleanup([file] { fclose(file); });
    // File will be closed automatically when cleanup goes out of scope
}
```