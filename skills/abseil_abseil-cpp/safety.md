# Safety

**Red Line 1: NEVER use absl::StatusOr without checking ok()**

BAD:
```cpp
absl::StatusOr<int> result = risky_operation();
int value = *result; // CRASH if result is error
```

GOOD:
```cpp
absl::StatusOr<int> result = risky_operation();
if (result.ok()) {
    int value = *result;
} else {
    // Handle error
}
```

**Red Line 2: NEVER use absl::string_view with temporary strings**

BAD:
```cpp
absl::string_view sv = std::string("temp"); // Dangling pointer
use_string_view(sv); // Undefined behavior
```

GOOD:
```cpp
std::string s = "permanent";
absl::string_view sv = s; // Safe
use_string_view(sv);
```

**Red Line 3: NEVER ignore absl::Status return values**

BAD:
```cpp
absl::Status status = do_something(); // Status ignored
// Continue without checking if operation succeeded
```

GOOD:
```cpp
absl::Status status = do_something();
if (!status.ok()) {
    LOG(ERROR) << "Operation failed: " << status;
    return status;
}
```

**Red Line 4: NEVER use absl::Mutex without proper locking**

BAD:
```cpp
absl::Mutex mu;
int shared_data;
void unsafe_access() {
    shared_data = 42; // No lock held - data race
}
```

GOOD:
```cpp
absl::Mutex mu;
int shared_data;
void safe_access() {
    absl::MutexLock lock(&mu);
    shared_data = 42; // Safe
}
```

**Red Line 5: NEVER use absl::Cleanup with captured references that may become invalid**

BAD:
```cpp
int* ptr = new int(42);
absl::Cleanup cleanup = [ptr] { delete ptr; };
delete ptr; // Double delete when cleanup runs
```

GOOD:
```cpp
auto ptr = std::make_unique<int>(42);
absl::Cleanup cleanup = [ptr = std::move(ptr)] { 
    // ptr will be deleted automatically when cleanup runs
};
```