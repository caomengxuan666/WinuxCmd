# Lifecycle

```cpp
// Construction and destruction of http_client
{
    http_client client(U("https://api.example.com")); // Constructed
    client.request(methods::GET, U("/data")).wait();
} // Destructor called - waits for pending operations

// Explicit destruction order
http_client client(U("https://api.example.com"));
auto task = client.request(methods::GET, U("/data"));
// Ensure task completes before client destruction
task.wait();
// client destroyed safely
```

```cpp
// Move semantics for http_client
http_client createClient() {
    return http_client(U("https://api.example.com"));
}

http_client client1(U("https://api1.example.com"));
http_client client2 = std::move(client1); // Move construction
// client1 is now in valid but unspecified state

// Move assignment
http_client client3(U("https://api3.example.com"));
client3 = std::move(client2); // Move assignment
```

```cpp
// Resource management for http_listener
class MyService {
private:
    http_listener m_listener;
    
public:
    MyService() : m_listener(U("http://localhost:8080")) {}
    
    pplx::task<void> start() {
        m_listener.support(methods::GET, [this](http_request request) {
            handleRequest(request);
        });
        return m_listener.open();
    }
    
    pplx::task<void> stop() {
        return m_listener.close();
    }
    
    ~MyService() {
        // Ensure listener is closed
        if (m_listener.is_open()) {
            m_listener.close().wait();
        }
    }
};
```

```cpp
// Proper cleanup of WebSocket connections
class WebSocketClient {
private:
    websocket_client m_client;
    bool m_connected = false;
    
public:
    pplx::task<void> connect(const std::string& uri) {
        return m_client.connect(U(uri))
            .then([this]() { m_connected = true; });
    }
    
    pplx::task<void> disconnect() {
        if (m_connected) {
            return m_client.close()
                .then([this]() { m_connected = false; });
        }
        return pplx::task_from_result();
    }
    
    ~WebSocketClient() {
        if (m_connected) {
            m_client.close().wait();
        }
    }
};
```