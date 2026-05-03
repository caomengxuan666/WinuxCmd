# Quickstart

```cpp
#include <iostream>
#include <string>
#include <vector>
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "absl/random/random.h"
#include "absl/time/time.h"
#include "absl/flags/flag.h"
#include "absl/log/log.h"

ABSL_FLAG(std::string, name, "World", "Name to greet");

// Pattern 1: String splitting
void string_split_example() {
    std::string input = "apple,banana,cherry";
    std::vector<std::string> parts = absl::StrSplit(input, ',');
    for (const auto& part : parts) {
        std::cout << part << std::endl;
    }
}

// Pattern 2: String joining
void string_join_example() {
    std::vector<std::string> words = {"Hello", "World", "from", "Abseil"};
    std::string result = absl::StrJoin(words, " ");
    std::cout << result << std::endl;
}

// Pattern 3: Flat hash map
void flat_hash_map_example() {
    absl::flat_hash_map<std::string, int> ages;
    ages["Alice"] = 30;
    ages["Bob"] = 25;
    for (const auto& [name, age] : ages) {
        std::cout << name << ": " << age << std::endl;
    }
}

// Pattern 4: StatusOr for error handling
absl::StatusOr<int> divide(int a, int b) {
    if (b == 0) {
        return absl::InvalidArgumentError("Division by zero");
    }
    return a / b;
}

void status_or_example() {
    auto result = divide(10, 2);
    if (result.ok()) {
        std::cout << "Result: " << *result << std::endl;
    } else {
        std::cout << "Error: " << result.status().message() << std::endl;
    }
}

// Pattern 5: Random number generation
void random_example() {
    absl::BitGen gen;
    int random_number = absl::Uniform(gen, 1, 100);
    std::cout << "Random number: " << random_number << std::endl;
}

// Pattern 6: Time operations
void time_example() {
    absl::Time now = absl::Now();
    absl::Duration duration = absl::Hours(2);
    absl::Time later = now + duration;
    std::cout << "Now: " << absl::FormatTime(now) << std::endl;
    std::cout << "Later: " << absl::FormatTime(later) << std::endl;
}

// Pattern 7: Logging
void logging_example() {
    LOG(INFO) << "This is an info message";
    LOG(WARNING) << "This is a warning";
    CHECK(1 + 1 == 2) << "Basic math failed!";
}

// Pattern 8: Command line flags
void flags_example() {
    std::string name = absl::GetFlag(FLAGS_name);
    std::cout << "Hello, " << name << "!" << std::endl;
}

int main() {
    string_split_example();
    string_join_example();
    flat_hash_map_example();
    status_or_example();
    random_example();
    time_example();
    logging_example();
    flags_example();
    return 0;
}
```