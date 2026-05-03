# Pitfalls


### Pitfall 1: Forgetting to set subscription filter on SUB sockets
```cpp
// BAD: SUB socket without subscription filter
zmq::context_t ctx;
zmq::socket_t sub(ctx, zmq::socket_type::sub);
sub.connect("inproc://test");
zmq::message_t msg;
sub.recv(msg, zmq::recv_flags::none); // Will never receive messages!

// GOOD: Always set subscription filter
zmq::context_t ctx;
zmq::socket_t sub(ctx, zmq::socket_type::sub);
sub.connect("inproc://test");
sub.set(zmq::sockopt::subscribe, ""); // Subscribe to all messages
// Or subscribe to specific prefix:
sub.set(zmq::sockopt::subscribe, "topic");
zmq::message_t msg;
sub.recv(msg, zmq::recv_flags::none);
```

### Pitfall 2: Using REQ/REP out of order
```cpp
// BAD: Sending multiple requests without replies
zmq::context_t ctx;
zmq::socket_t req(ctx, zmq::socket_type::req);
req.connect("inproc://test");
req.send(zmq::str_buffer("req1"), zmq::send_flags::none);
req.send(zmq::str_buffer("req2"), zmq::send_flags::none); // Will throw!

// GOOD: Strict request-reply alternation
zmq::context_t ctx;
zmq::socket_t req(ctx, zmq::socket_type::req);
req.connect("inproc://test");
req.send(zmq::str_buffer("req1"), zmq::send_flags::none);
zmq::message_t reply;
req.recv(reply, zmq::recv_flags::none);
req.send(zmq::str_buffer("req2"), zmq::send_flags::none);
```

### Pitfall 3: Ignoring send/recv return values
```cpp
// BAD: Ignoring return values
zmq::socket_t sock(ctx, zmq::socket_type::push);
sock.send(zmq::str_buffer("data"), zmq::send_flags::dontwait);

// GOOD: Checking return values
zmq::socket_t sock(ctx, zmq::socket_type::push);
auto result = sock.send(zmq::str_buffer("data"), zmq::send_flags::dontwait);
if (!result) {
    // Handle failure (e.g., would block)
    std::cerr << "Send failed" << std::endl;
}
```

### Pitfall 4: Using socket after context destruction
```cpp
// BAD: Context destroyed before socket
zmq::socket_t* sock;
{
    zmq::context_t ctx;
    sock = new zmq::socket_t(ctx, zmq::socket_type::push);
} // ctx destroyed here
sock->send(zmq::str_buffer("test"), zmq::send_flags::none); // Undefined behavior!

// GOOD: Context outlives sockets
zmq::context_t ctx;
{
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.send(zmq::str_buffer("test"), zmq::send_flags::none);
} // sock destroyed first, then ctx
```

### Pitfall 5: Mixing inproc and TCP endpoints incorrectly
```cpp
// BAD: Using inproc across different contexts
zmq::context_t ctx1;
zmq::context_t ctx2;
zmq::socket_t pub(ctx1, zmq::socket_type::pub);
zmq::socket_t sub(ctx2, zmq::socket_type::sub);
pub.bind("inproc://test");
sub.connect("inproc://test"); // Won't work! Different contexts

// GOOD: Same context for inproc
zmq::context_t ctx;
zmq::socket_t pub(ctx, zmq::socket_type::pub);
zmq::socket_t sub(ctx, zmq::socket_type::sub);
pub.bind("inproc://test");
sub.connect("inproc://test");
```

### Pitfall 6: Not handling EAGAIN properly
```cpp
// BAD: Assuming send always succeeds
zmq::socket_t sock(ctx, zmq::socket_type::push);
sock.send(zmq::str_buffer("data"), zmq::send_flags::dontwait);

// GOOD: Handle EAGAIN with retry or backpressure
zmq::socket_t sock(ctx, zmq::socket_type::push);
while (true) {
    try {
        sock.send(zmq::str_buffer("data"), zmq::send_flags::dontwait);
        break;
    } catch (const zmq::error_t& e) {
        if (e.num() != EAGAIN) throw;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
```

### Pitfall 7: Forgetting to close sockets properly
```cpp
// BAD: Relying on destructor order in complex scenarios
{
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.bind("tcp://*:5555");
    // Socket closes when going out of scope
}

// GOOD: Explicit close when needed
{
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.bind("tcp://*:5555");
    // Use socket...
    sock.close(); // Explicit close
}
```

### Pitfall 8: Using message_t after moving from it
```cpp
// BAD: Using moved-from message
zmq::message_t msg1;
zmq::message_t msg2 = std::move(msg1);
msg1.data(); // Undefined behavior!

// GOOD: Only use message after move if reassigned
zmq::message_t msg1;
zmq::message_t msg2 = std::move(msg1);
msg1 = zmq::message_t(); // Reassign before use
msg1.data(); // OK now
```