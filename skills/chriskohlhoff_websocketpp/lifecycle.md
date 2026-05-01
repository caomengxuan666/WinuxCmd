# Lifecycle

**Construction and Initialization**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

// Default construction
websocketpp::server<websocketpp::config::asio> server;

// Construction with custom configuration
struct MyConfig : public websocketpp::config::asio {
    static const bool enable_verbose_logging = false;
};
websocketpp::server<MyConfig> custom_server;

// Initialization sequence
server.init_asio();                    // Must be called first
server.set_reuse_addr(true);           // Optional: reuse address
server.listen(9002);                   // Bind to port
server.start_accept();                 // Start accepting connections
server.run();                          // Enter event loop (blocking)
```

**Connection Lifecycle**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

websocketpp::server<websocketpp::config::asio> server;

// Connection lifecycle handlers
server.set_validate_handler([](websocketpp::connection_hdl hdl) {
    // 1. Validation phase - before connection is established
    std::cout << "Validating new connection" << std::endl;
    return true; // Accept connection
});

server.set_open_handler([](websocketpp::connection_hdl hdl) {
    // 2. Connection established
    std::cout << "New connection opened" << std::endl;
});

server.set_message_handler([](websocketpp::connection_hdl hdl, 
                               websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    // 3. Message received
    std::cout << "Message: " << msg->get_payload() << std::endl;
});

server.set_close_handler([](websocketpp::connection_hdl hdl) {
    // 4. Connection closing
    std::cout << "Connection closed" << std::endl;
});

server.set_fail_handler([](websocketpp::connection_hdl hdl) {
    // 5. Connection failed
    std::cout << "Connection failed" << std::endl;
});
```

**Resource Management and Destruction**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <memory>

class WebSocketServer {
public:
    WebSocketServer() : m_server(std::make_unique<server>()) {
        m_server->init_asio();
    }
    
    ~WebSocketServer() {
        shutdown();
    }
    
    void shutdown() {
        if (m_server) {
            m_server->stop_listening();
            m_server->stop();
            // Wait for all connections to close
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    // Move constructor
    WebSocketServer(WebSocketServer&& other) noexcept 
        : m_server(std::move(other.m_server)) {}
    
    // Move assignment
    WebSocketServer& operator=(WebSocketServer&& other) noexcept {
        if (this != &other) {
            shutdown();
            m_server = std::move(other.m_server);
        }
        return *this;
    }
    
    // No copy semantics
    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;

private:
    using server = websocketpp::server<websocketpp::config::asio>;
    std::unique_ptr<server> m_server;
};
```

**Connection Handle Management**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <memory>
#include <set>

websocketpp::server<websocketpp::config::asio> server;

// Safe connection handle storage
std::set<websocketpp::connection_hdl, 
         std::owner_less<websocketpp::connection_hdl>> connections;

server.set_open_handler([](websocketpp::connection_hdl hdl) {
    connections.insert(hdl);
});

server.set_close_handler([](websocketpp::connection_hdl hdl) {
    connections.erase(hdl);
});

// Safe access pattern
void broadcast(const std::string& message) {
    for (auto& hdl : connections) {
        try {
            server.send(hdl, message, websocketpp::frame::opcode::text);
        } catch (const websocketpp::exception& e) {
            // Handle invalid handle
            connections.erase(hdl);
        }
    }
}
```