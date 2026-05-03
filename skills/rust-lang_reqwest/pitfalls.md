# Pitfalls

### 1. Not handling network errors
```cpp
// BAD: No error handling
auto response = reqwest::get("https://api.example.com/data");
std::cout << response.text(); // Crashes if network fails

// GOOD: Proper error handling
try {
    auto response = reqwest::get("https://api.example.com/data");
    if (response.status_code() == 200) {
        std::cout << response.text();
    } else {
        std::cerr << "HTTP error: " << response.status_code();
    }
} catch (const std::exception& e) {
    std::cerr << "Request failed: " << e.what();
}
```

### 2. Forgetting to set Content-Type header
```cpp
// BAD: Missing Content-Type
auto response = client.post("https://api.example.com/data")
    .body("{\"key\":\"value\"}")
    .send();

// GOOD: Explicit Content-Type
auto response = client.post("https://api.example.com/data")
    .header("Content-Type", "application/json")
    .body("{\"key\":\"value\"}")
    .send();
```

### 3. Reusing client without timeout
```cpp
// BAD: No timeout, could hang forever
auto client = reqwest::client();
auto response = client.get("https://slow-api.example.com").send();

// GOOD: Set reasonable timeout
auto client = reqwest::client_builder()
    .timeout(std::chrono::seconds(30))
    .build();
auto response = client.get("https://slow-api.example.com").send();
```

### 4. Ignoring response status codes
```cpp
// BAD: Assuming success
auto response = client.get("https://api.example.com/data").send();
process_data(response.text()); // Might process error page

// GOOD: Check status code
auto response = client.get("https://api.example.com/data").send();
if (response.status_code() == 200) {
    process_data(response.text());
} else {
    handle_error(response.status_code());
}
```

### 5. Not closing response body
```cpp
// BAD: Large response not consumed
auto response = client.get("https://example.com/large-file").send();
// response goes out of scope, but body not fully read

// GOOD: Read body explicitly
auto response = client.get("https://example.com/large-file").send();
std::string body = response.text(); // Consumes the body
```

### 6. Using default headers incorrectly
```cpp
// BAD: Overwriting default headers
auto response = client.get("https://api.example.com")
    .header("Accept", "text/html") // Overwrites default
    .send();

// GOOD: Understanding header behavior
auto response = client.get("https://api.example.com")
    .header("Accept", "application/json")
    .send();
```

### 7. Not handling redirects
```cpp
// BAD: Following redirects blindly
auto response = client.get("https://example.com/redirect").send(); // May follow to malicious site

// GOOD: Control redirect behavior
auto client = reqwest::client_builder()
    .redirect(reqwest::redirect::policy::limited(5))
    .build();
auto response = client.get("https://example.com/redirect").send();
```

### 8. Memory leak with large responses
```cpp
// BAD: Loading entire response into memory
auto response = client.get("https://example.com/huge-file").send();
std::string huge_data = response.text(); // May OOM

// GOOD: Stream large responses
auto response = client.get("https://example.com/huge-file").send();
std::ofstream file("output.bin", std::ios::binary);
file << response.text(); // Still loads all, but writes to file
// Better: use chunked reading if available
```