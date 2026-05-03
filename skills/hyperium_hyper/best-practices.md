# Best Practices

Use connection pooling for optimal performance:
```cpp
#include <hyper/client.hpp>

// Create client once and reuse
hyper::client::Client client;

// Configure pool size
hyper::client::Config config;
config.set_max_connections(100);
config.set_max_connections_per_host(10);

hyper::client::Client pooled_client(config);
```

Implement proper error handling:
```cpp
#include <hyper/client.hpp>
#include <iostream>

hyper::client::Client client;

auto make_request_with_retry(const std::string& url, int max_retries = 3) {
    for (int i = 0; i < max_retries; ++i) {
        try {
            auto req = hyper::client::Request::builder()
                .method(hyper::Method::GET)
                .uri(url)
                .build();
            
            auto response = client.request(req).get();
            if (response.status() < 500) {
                return response;
            }
        } catch (const hyper::TimeoutError& e) {
            std::cerr << "Timeout on attempt " << i + 1 << std::endl;
        } catch (const hyper::NetworkError& e) {
            std::cerr << "Network error on attempt " << i + 1 << std::endl;
        }
    }
    throw std::runtime_error("Max retries exceeded");
}
```

Use streaming for large payloads:
```cpp
#include <hyper/client.hpp>
#include <fstream>

void download_large_file(const std::string& url, const std::string& output_path) {
    hyper::client::Client client;
    
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri(url)
        .build();
    
    auto response = client.request(req).get();
    auto body = response.into_body();
    
    std::ofstream file(output_path, std::ios::binary);
    while (auto chunk = body.data()) {
        file.write(chunk->as_bytes().data(), chunk->len());
    }
}
```

Implement proper header management:
```cpp
#include <hyper/client.hpp>

hyper::client::Client client;

auto create_authenticated_request(const std::string& url, const std::string& token) {
    return hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri(url)
        .header("Authorization", "Bearer " + token)
        .header("Accept", "application/json")
        .header("User-Agent", "MyApp/1.0")
        .build();
}
```