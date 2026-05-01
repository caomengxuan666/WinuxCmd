# Safety

**Red Line 1: Never Access Connection After Close**
```cpp
// BAD: Using connection after close
server::connection_ptr con = server.get_con_from_hdl(hdl);
server.close(hdl, websocketpp::close::status::normal, "Closing");
con->send("data"); // UNDEFINED BEHAVIOR - connection is invalid

// GOOD: Check connection state before use
server::connection_ptr con = server.get_con_from_hdl(hdl);
if (con && con->get_state() == websocketpp::session::state::open) {
    con->send("data", websocketpp::frame::opcode::text);
}
```

**Red Line 2: Never Call send() from Multiple Threads Without Synchronization**
```cpp
// BAD: Concurrent sends from multiple threads
void thread1() { server.send(hdl, "msg1", websocketpp::frame::opcode::text); }
void thread2() { server.send(hdl, "msg2", websocketpp::frame::opcode::text); }
// Data race on internal buffers!

// GOOD: Serialize all sends through a single thread or mutex
std::mutex send_mutex;
void safe_send(websocketpp::connection_hdl hdl, const std::string& msg) {
    std::lock_guard<std::mutex> lock(send_mutex);
    server.send(hdl, msg, websocketpp::frame::opcode::text);
}
```

**Red Line 3: Never Use get_con_from_hdl() After Connection is Destroyed**
```cpp
// BAD: Storing connection pointer and using after destruction
server::connection_ptr stored_con;
server.set_open_handler([&stored_con](websocketpp::connection_hdl hdl) {
    stored_con = server.get_con_from_hdl(hdl);
});
// Later, connection closes and stored_con becomes dangling
stored_con->send("data"); // CRASH!

// GOOD: Always use connection_hdl and check existence
websocketpp::connection_hdl stored_hdl;
server.set_open_handler([&stored_hdl](websocketpp::connection_hdl hdl) {
    stored_hdl = hdl;
});
// Check before use
try {
    server.send(stored_hdl, "data", websocketpp::frame::opcode::text);
} catch (const websocketpp::exception& e) {
    // Handle invalid handle
}
```

**Red Line 4: Never Ignore TLS Verification Errors**
```cpp
// BAD: Disabling TLS verification
server.set_tls_init_handler([](websocketpp::connection_hdl) {
    auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(
        boost::asio::ssl::context::tlsv12);
    ctx->set_verify_mode(boost::asio::ssl::verify_none); // INSECURE!
    return ctx;
});

// GOOD: Proper TLS verification
server.set_tls_init_handler([](websocketpp::connection_hdl) {
    auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(
        boost::asio::ssl::context::tlsv12);
    ctx->set_verify_mode(boost::asio::ssl::verify_peer |
                         boost::asio::ssl::verify_fail_if_no_peer_cert);
    ctx->load_verify_file("ca_cert.pem");
    return ctx;
});
```

**Red Line 5: Never Call run() Multiple Times on Same Endpoint**
```cpp
// BAD: Multiple run() calls
server ws_server;
ws_server.init_asio();
ws_server.listen(9002);
ws_server.start_accept();
std::thread t1([&ws_server]() { ws_server.run(); });
std::thread t2([&ws_server]() { ws_server.run(); }); // UNDEFINED BEHAVIOR!

// GOOD: Use stop() and restart properly
server ws_server;
ws_server.init_asio();
ws_server.listen(9002);
ws_server.start_accept();
std::thread worker([&ws_server]() { ws_server.run(); });
// To restart:
ws_server.stop();
worker.join();
ws_server.init_asio(); // Reinitialize
ws_server.listen(9002);
ws_server.start_accept();
worker = std::thread([&ws_server]() { ws_server.run(); });
```