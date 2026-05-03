# Safety

### 1. NEVER ignore SSL/TLS verification
```cpp
// BAD: Disabling SSL verification
auto client = reqwest::client_builder()
    .danger_accept_invalid_certs(true) // NEVER do this in production
    .build();

// GOOD: Use proper SSL verification
auto client = reqwest::client_builder()
    .use_default_tls() // Uses system certificates
    .build();
```

### 2. NEVER hardcode credentials in source code
```cpp
// BAD: Hardcoded credentials
auto response = client.get("https://api.example.com")
    .header("Authorization", "Bearer my-secret-token-12345")
    .send();

// GOOD: Use environment variables or secure config
#include <cstdlib>
std::string token = std::getenv("API_TOKEN");
auto response = client.get("https://api.example.com")
    .header("Authorization", "Bearer " + token)
    .send();
```

### 3. NEVER use user input directly in URLs
```cpp
// BAD: Direct user input in URL
std::string user_input = get_user_input();
auto response = client.get("https://api.example.com/" + user_input).send(); // Path traversal risk

// GOOD: Validate and encode user input
std::string user_input = get_user_input();
if (is_valid_input(user_input)) {
    auto encoded = reqwest::url_encode(user_input);
    auto response = client.get("https://api.example.com/" + encoded).send();
}
```

### 4. NEVER ignore response body for error codes
```cpp
// BAD: Ignoring error response body
auto response = client.get("https://api.example.com/data").send();
if (response.status_code() != 200) {
    return; // Lost error details
}

// GOOD: Read error body for debugging
auto response = client.get("https://api.example.com/data").send();
if (response.status_code() != 200) {
    std::string error_body = response.text();
    log_error("Request failed: " + error_body);
    return;
}
```

### 5. NEVER use default timeout for critical operations
```cpp
// BAD: No timeout for critical operation
auto response = client.get("https://critical-service.example.com").send(); // May hang forever

// GOOD: Set appropriate timeout
auto client = reqwest::client_builder()
    .timeout(std::chrono::seconds(5)) // Fail fast
    .connect_timeout(std::chrono::seconds(2))
    .build();
auto response = client.get("https://critical-service.example.com").send();
```