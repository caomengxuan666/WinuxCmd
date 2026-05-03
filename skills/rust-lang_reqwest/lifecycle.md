# Lifecycle

### Construction
```cpp
// Default client construction
reqwest::client client; // Creates client with default settings

// Custom client construction
reqwest::client client = reqwest::client_builder()
    .timeout(std::chrono::seconds(30))
    .default_headers({{"User-Agent", "MyApp/1.0"}})
    .build();

// Client construction is expensive - reuse when possible
```

### Destruction
```cpp
// Client destructor closes all connections
{
    reqwest::client client;
    auto response = client.get("https://example.com").send();
    // Client destructor called here - waits for pending requests
} // All connections closed

// Response destructor
{
    auto response = reqwest::get("https://example.com").send();
    // Response destructor called - body may not be fully read
} // Memory freed, but body discarded
```

### Move Semantics
```cpp
// Client is movable but not copyable
reqwest::client client1 = reqwest::client_builder().build();
reqwest::client client2 = std::move(client1); // OK: move
// reqwest::client client3 = client1; // ERROR: copy not allowed

// Response is movable
auto response1 = reqwest::get("https://example.com").send();
auto response2 = std::move(response1); // OK: move
// auto response3 = response1; // ERROR: copy not allowed
```

### Resource Management
```cpp
// Proper resource cleanup
void fetch_data() {
    reqwest::client client; // Resource acquired
    
    // Multiple requests reuse same client
    auto r1 = client.get("https://api1.example.com").send();
    auto r2 = client.get("https://api2.example.com").send();
    
    // Ensure responses are consumed
    std::string data1 = r1.text();
    std::string data2 = r2.text();
    
    // Client destructor releases connections
} // All resources cleaned up

// Avoid resource leaks
void bad_example() {
    auto client = std::make_unique<reqwest::client>(); // Heap allocation unnecessary
    auto response = client->get("https://example.com").send();
    // Forgot to delete client - memory leak
}
```