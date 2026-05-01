# Pitfalls

**Pitfall 1: Not Checking Error Codes**
```cpp
// BAD: Ignoring error codes
client::connection_ptr con = ws_client.get_connection("ws://invalid.url");
ws_client.connect(con); // Crashes if connection fails

// GOOD: Always check error codes
websocketpp::lib::error_code ec;
client::connection_ptr con = ws_client.get_connection("ws://invalid.url", ec);
if (ec) {
    std::cerr << "Connection error: " << ec.message() << std::endl;
    return;
}
ws_client.connect(con);
```

**Pitfall 2: Accessing Connection After Close**
```cpp
// BAD: Using connection after close handler
server::connection_ptr con;
server.set_close_handler([&con](websocketpp::connection_hdl hdl) {
    con = server.get_con_from_hdl(hdl);
    // con is now invalid
});
// Later: con->send("data"); // Undefined behavior

// GOOD: Store connection handle, not pointer
websocketpp::connection_hdl saved_hdl;
server.set_open_handler([&saved_hdl](websocketpp::connection_hdl hdl) {
    saved_hdl = hdl;
});
// Use handle with server methods
server.send(saved_hdl, "data", websocketpp::frame::opcode::text);
```

**Pitfall 3: Thread Safety Violations**
```cpp
// BAD: Concurrent access without synchronization
std::set<websocketpp::connection_hdl> connections;
// Multiple threads call this:
connections.insert(hdl); // Data race!

// GOOD: Protect shared state with mutex
std::mutex mtx;
std::set<websocketpp::connection_hdl> connections;
// Thread-safe access:
{
    std::lock_guard<std::mutex> lock(mtx);
    connections.insert(hdl);
}
```

**Pitfall 4: Not Handling Large Messages**
```cpp
// BAD: No message size limit
server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
    process(msg->get_payload()); // Could be gigabytes!
});

// GOOD: Check message size
server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
    if (msg->get_payload().size() > MAX_MESSAGE_SIZE) {
        server.close(hdl, websocketpp::close::status::message_too_big, "Message too large");
        return;
    }
    process(msg->get_payload());
});
```

**Pitfall 5: Forgetting to Initialize Asio**
```cpp
// BAD: Missing init_asio() call
server ws_server;
ws_server.listen(9002); // Crashes - no io_service

// GOOD: Always initialize Asio first
server ws_server;
ws_server.init_asio();
ws_server.listen(9002);
ws_server.start_accept();
ws_server.run();
```

**Pitfall 6: Blocking in Handlers**
```cpp
// BAD: Blocking in message handler
server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Blocks event loop!
    server.send(hdl, "response", websocketpp::frame::opcode::text);
});

// GOOD: Use async operations or thread pool
server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::thread([hdl, msg]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        server.send(hdl, "response", websocketpp::frame::opcode::text);
    }).detach();
});
```

**Pitfall 7: Not Setting Proper Close Codes**
```cpp
// BAD: Using invalid close codes
server.close(hdl, 9999, "Custom reason"); // Invalid close code

// GOOD: Use standard close codes
server.close(hdl, websocketpp::close::status::normal, "Normal closure");
server.close(hdl, websocketpp::close::status::going_away, "Server shutting down");
server.close(hdl, websocketpp::close::status::protocol_error, "Protocol violation");
```

**Pitfall 8: Memory Leaks from Shared Pointers**
```cpp
// BAD: Circular references with shared_ptr
auto handler = std::make_shared<MyHandler>();
handler->server = &ws_server; // Potential dangling pointer

// GOOD: Use weak_ptr or raw pointers carefully
std::weak_ptr<MyHandler> weak_handler = handler;
ws_server.set_message_handler([weak_handler](websocketpp::connection_hdl hdl, server::message_ptr msg) {
    if (auto handler = weak_handler.lock()) {
        handler->onMessage(msg);
    }
});
```