# Quickstart

```cpp
// WebSocket++ Quickstart Examples
// Copy-paste ready examples for common patterns

// Example 1: Simple Echo Server
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

int main() {
    server echo_server;
    
    echo_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        echo_server.send(hdl, msg->get_payload(), msg->get_opcode());
    });
    
    echo_server.init_asio();
    echo_server.listen(9002);
    echo_server.start_accept();
    echo_server.run();
    return 0;
}
```

```cpp
// Example 2: Simple Client Connection
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_client> client;

int main() {
    client ws_client;
    
    ws_client.set_message_handler([](websocketpp::connection_hdl hdl, client::message_ptr msg) {
        std::cout << "Received: " << msg->get_payload() << std::endl;
    });
    
    ws_client.init_asio();
    
    websocketpp::lib::error_code ec;
    client::connection_ptr con = ws_client.get_connection("ws://echo.websocket.org", ec);
    if (ec) {
        std::cout << "Connection error: " << ec.message() << std::endl;
        return 1;
    }
    
    ws_client.connect(con);
    ws_client.run();
    return 0;
}
```

```cpp
// Example 3: TLS/SSL Server
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio_tls> server;

int main() {
    server tls_server;
    
    tls_server.set_tls_init_handler([](websocketpp::connection_hdl) {
        auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(
            boost::asio::ssl::context::tlsv12);
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                        boost::asio::ssl::context::no_sslv2 |
                        boost::asio::ssl::context::single_dh_use);
        ctx->use_certificate_chain_file("server.pem");
        ctx->use_private_key_file("server.key", boost::asio::ssl::context::pem);
        return ctx;
    });
    
    tls_server.init_asio();
    tls_server.listen(9002);
    tls_server.start_accept();
    tls_server.run();
    return 0;
}
```

```cpp
// Example 4: Broadcasting to All Connections
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <mutex>

typedef websocketpp::server<websocketpp::config::asio> server;
std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;
std::mutex connections_mutex;

int main() {
    server broadcast_server;
    
    broadcast_server.set_open_handler([](websocketpp::connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(connections_mutex);
        connections.insert(hdl);
    });
    
    broadcast_server.set_close_handler([](websocketpp::connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(connections_mutex);
        connections.erase(hdl);
    });
    
    broadcast_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::lock_guard<std::mutex> lock(connections_mutex);
        for (auto& conn : connections) {
            broadcast_server.send(conn, msg->get_payload(), msg->get_opcode());
        }
    });
    
    broadcast_server.init_asio();
    broadcast_server.listen(9002);
    broadcast_server.start_accept();
    broadcast_server.run();
    return 0;
}
```

```cpp
// Example 5: Ping/Pong Keep-Alive
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

int main() {
    server ping_server;
    
    ping_server.set_ping_handler([](websocketpp::connection_hdl hdl, std::string payload) {
        std::cout << "Ping received: " << payload << std::endl;
        return true; // Automatically sends pong
    });
    
    ping_server.set_pong_handler([](websocketpp::connection_hdl hdl, std::string payload) {
        std::cout << "Pong received: " << payload << std::endl;
    });
    
    ping_server.set_pong_timeout_handler([](websocketpp::connection_hdl hdl, std::string payload) {
        std::cout << "Pong timeout - closing connection" << std::endl;
        ping_server.close(hdl, websocketpp::close::status::going_away, "Pong timeout");
    });
    
    ping_server.init_asio();
    ping_server.listen(9002);
    ping_server.start_accept();
    ping_server.run();
    return 0;
}
```

```cpp
// Example 6: Custom Message Validation
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <regex>

typedef websocketpp::server<websocketpp::config::asio> server;

int main() {
    server validate_server;
    
    validate_server.set_validate_handler([](websocketpp::connection_hdl hdl) {
        server::connection_ptr con = validate_server.get_con_from_hdl(hdl);
        std::string origin = con->get_origin();
        std::string host = con->get_host();
        
        // Validate origin
        if (origin != "https://myapp.com") {
            std::cout << "Rejected connection from: " << origin << std::endl;
            return false;
        }
        return true;
    });
    
    validate_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::string payload = msg->get_payload();
        
        // Validate message size
        if (payload.size() > 1024) {
            std::cout << "Message too large" << std::endl;
            return;
        }
        
        validate_server.send(hdl, payload, msg->get_opcode());
    });
    
    validate_server.init_asio();
    validate_server.listen(9002);
    validate_server.start_accept();
    validate_server.run();
    return 0;
}
```

```cpp
// Example 7: Multi-threaded Server
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include <vector>

typedef websocketpp::server<websocketpp::config::asio> server;

int main() {
    server multi_thread_server;
    
    multi_thread_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        multi_thread_server.send(hdl, msg->get_payload(), msg->get_opcode());
    });
    
    multi_thread_server.init_asio();
    multi_thread_server.set_reuse_addr(true);
    multi_thread_server.listen(9002);
    multi_thread_server.start_accept();
    
    // Run in multiple threads
    std::vector<std::thread> threads;
    unsigned int thread_count = std::thread::hardware_concurrency();
    
    for (unsigned int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&multi_thread_server]() {
            multi_thread_server.run();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

```cpp
// Example 8: Subprotocol Negotiation
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

int main() {
    server subprotocol_server;
    
    subprotocol_server.set_open_handler([](websocketpp::connection_hdl hdl) {
        server::connection_ptr con = subprotocol_server.get_con_from_hdl(hdl);
        std::string subprotocol = con->get_subprotocol();
        std::cout << "Client selected subprotocol: " << subprotocol << std::endl;
    });
    
    subprotocol_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        subprotocol_server.send(hdl, msg->get_payload(), msg->get_opcode());
    });
    
    subprotocol_server.init_asio();
    subprotocol_server.listen(9002);
    subprotocol_server.start_accept();
    subprotocol_server.run();
    return 0;
}
```