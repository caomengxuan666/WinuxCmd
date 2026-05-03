# Quickstart

```cpp
#include <reqwest/reqwest.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

// 1. Simple GET request
void simple_get() {
    auto response = reqwest::get("https://api.example.com/data");
    std::cout << response.text() << std::endl;
}

// 2. GET with query parameters
void get_with_params() {
    auto client = reqwest::client();
    auto response = client.get("https://api.example.com/search")
        .query({{"q", "rust"}, {"limit", "10"}})
        .send();
    std::cout << response.text() << std::endl;
}

// 3. POST with JSON body
void post_json() {
    auto client = reqwest::client();
    nlohmann::json body = {{"name", "John"}, {"age", 30}};
    auto response = client.post("https://api.example.com/users")
        .header("Content-Type", "application/json")
        .body(body.dump())
        .send();
    std::cout << response.status_code() << std::endl;
}

// 4. POST with form data
void post_form() {
    auto client = reqwest::client();
    auto response = client.post("https://api.example.com/login")
        .form({{"username", "admin"}, {"password", "secret"}})
        .send();
    std::cout << response.text() << std::endl;
}

// 5. Download file to disk
void download_file() {
    auto response = reqwest::get("https://example.com/file.zip");
    std::ofstream file("file.zip", std::ios::binary);
    file << response.text();
    file.close();
}

// 6. Custom headers and timeout
void custom_headers() {
    auto client = reqwest::client_builder()
        .timeout(std::chrono::seconds(30))
        .build();
    auto response = client.get("https://api.example.com/protected")
        .header("Authorization", "Bearer token123")
        .header("User-Agent", "MyApp/1.0")
        .send();
    std::cout << response.text() << std::endl;
}

// 7. Error handling
void error_handling() {
    try {
        auto response = reqwest::get("https://api.example.com/data");
        if (response.status_code() == 200) {
            std::cout << "Success: " << response.text() << std::endl;
        } else {
            std::cerr << "Error: " << response.status_code() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Request failed: " << e.what() << std::endl;
    }
}

// 8. Async request (basic)
void async_request() {
    auto client = reqwest::client();
    auto future = client.get("https://api.example.com/data")
        .send_async();
    // Do other work...
    auto response = future.get();
    std::cout << response.text() << std::endl;
}
```