# Quickstart

```cpp
// Basic HTTP GET request
#include <hyper/client.hpp>
#include <hyper/body.hpp>
#include <iostream>

hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .body(hyper::Body::empty())
    .build();

auto response = client.request(req).get();
std::cout << "Status: " << response.status() << std::endl;
```

```cpp
// POST with JSON body
#include <hyper/client.hpp>
#include <hyper/body.hpp>
#include <string>

hyper::client::Client client;
std::string json_body = R"({"name":"test","value":42})";

auto req = hyper::client::Request::builder()
    .method(hyper::Method::POST)
    .uri("http://api.example.com/data")
    .header("Content-Type", "application/json")
    .body(hyper::Body::from(json_body))
    .build();

auto response = client.request(req).get();
```

```cpp
// Streaming response body
#include <hyper/client.hpp>
#include <hyper/body.hpp>
#include <iostream>

hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com/large-file")
    .build();

auto response = client.request(req).get();
auto body = response.into_body();

while (auto chunk = body.data()) {
    std::cout.write(chunk->as_bytes().data(), chunk->len());
}
```

```cpp
// Async request with callback
#include <hyper/client.hpp>
#include <hyper/future.hpp>

hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

client.request_async(req, [](hyper::Result<hyper::Response> result) {
    if (result.is_ok()) {
        auto response = result.unwrap();
        // handle response
    }
});
```

```cpp
// Custom timeout configuration
#include <hyper/client.hpp>
#include <chrono>

hyper::client::Config config;
config.set_connect_timeout(std::chrono::seconds(5));
config.set_read_timeout(std::chrono::seconds(30));

hyper::client::Client client(config);
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

auto response = client.request(req).get();
```

```cpp
// HTTPS with custom TLS settings
#include <hyper/client.hpp>
#include <hyper/tls.hpp>

hyper::tls::Config tls_config;
tls_config.set_verify(true);
tls_config.set_ca_file("/etc/ssl/certs/ca-certificates.crt");

hyper::client::Config config;
config.set_tls_config(tls_config);

hyper::client::Client client(config);
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("https://secure.example.com")
    .build();

auto response = client.request(req).get();
```