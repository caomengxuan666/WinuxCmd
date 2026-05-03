# Lifecycle

Construction and configuration:
```cpp
#include <hyper/client.hpp>

// Default construction
hyper::client::Client default_client;

// Configured construction
hyper::client::Config config;
config.set_connect_timeout(std::chrono::seconds(5));
config.set_read_timeout(std::chrono::seconds(30));
config.set_max_connections(50);

hyper::client::Client configured_client(config);
```

Move semantics:
```cpp
#include <hyper/client.hpp>

hyper::client::Client client1;
// Configure client1...

// Move construction - client1 becomes moved-from state
hyper::client::Client client2 = std::move(client1);

// Move assignment
hyper::client::Client client3;
client3 = std::move(client2);

// client1 and client2 are now in valid but unspecified state
// They can be destroyed or assigned to, but not used for requests
```

Resource management with RAII:
```cpp
#include <hyper/client.hpp>
#include <memory>

class HttpClient {
private:
    std::unique_ptr<hyper::client::Client> client;
    
public:
    HttpClient() : client(std::make_unique<hyper::client::Client>()) {}
    
    // No explicit destructor needed - unique_ptr handles cleanup
    
    void make_request(const std::string& url) {
        auto req = hyper::client::Request::builder()
            .method(hyper::Method::GET)
            .uri(url)
            .build();
        
        auto response = client->request(req).get();
        // Process response...
    }
};
```

Destruction and cleanup:
```cpp
#include <hyper/client.hpp>

void process_requests() {
    hyper::client::Client client;
    
    // Use client for multiple requests...
    {
        auto req = hyper::client::Request::builder()
            .method(hyper::Method::GET)
            .uri("http://example.com")
            .build();
        auto response = client.request(req).get();
    } // Request objects are destroyed here
    
    // More requests...
    
} // Client is destroyed here, all connections are cleaned up
```